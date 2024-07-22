#!/usr/bin/env python

import os
import re
import sys
from collections import defaultdict

fileNameMap = {"main.h": "kmain.h", "main.c++": "kmain.c++"}


def extract_headers_by_set(cmake_file: str, file_extension: str):
    if not os.path.isfile(cmake_file):
        print(f"File not found: {cmake_file}")
        return {}

    headers_by_set = defaultdict(list)
    current_set = None

    with open(cmake_file, "r") as file:
        for line in file:
            # set_match = re.match(r"\s*set\s*\(\s*([-\w]+)\s*", line, re.IGNORECASE)
            set_match = re.match(
                r"\s*(set|add_executable)\s*\(\s*([-\w]+)\s*", line, re.IGNORECASE
            )
            if set_match:
                print(f"set_match: {set_match}")
                current_set = set_match.group(2)
            elif current_set:
                header_matches = re.findall(rf'[^\s()"\']+\.{file_extension}', line)
                if len(header_matches) == 0:
                    continue
                headers_by_set[current_set].extend(header_matches)
                print(f"line_match: {header_matches}")
                if ")" in line:  # End of the current set definition
                    current_set = None

    return headers_by_set


def normalize_paths(headers, cmake_dir):
    normalized_headers = []
    for header in headers:
        header_path = os.path.normpath(os.path.join(cmake_dir, header))
        if os.path.isfile(header_path):
            # normalized_headers.append(os.path.abspath(header_path))
            normalized_headers.append(header)
    return normalized_headers


def create_subdirectories(header_set: str, target_path: str):
    tpath = os.path.join(target_path, header_set)
    if not os.path.exists(tpath):
        print(f"creating subdirectory {header_set} at {target_path}")
        os.makedirs(tpath)


def map_path(path: str):
    dirname = os.path.dirname(path)
    basename = os.path.basename(path)
    if basename in fileNameMap:
        mapped_name = fileNameMap[basename]
        print(f"mapping {basename} to {mapped_name}")
        return os.path.join(dirname, mapped_name)
    return path


def create_symlinks(headers: list[str], target_path: str, source_path: str):
    print(f"creating symlinks for headers in: {target_path}, {source_path}")
    for header_path in headers:
        sym_src = os.path.join(source_path, header_path)
        sym_dst = os.path.join(target_path, map_path(header_path))
        sym_dst_dir = os.path.dirname(sym_dst)
        if not os.path.exists(sym_dst_dir):
            os.makedirs(sym_dst_dir)
        sym_rel_src = os.path.relpath(sym_src, sym_dst_dir)
        # print(f"sym src: {sym_rel_src}, sym dst: {sym_dst_dir}")

        try:
            os.symlink(sym_rel_src, sym_dst)
            # print(
            # f"Created symlink: {symlink_relative_source_path} -> {symlink_target_path}"
            # )
        except FileExistsError:
            print(f"Symlink already exists: {sym_dst}")
        except OSError as e:
            print(f"Failed to create symlink: {sym_rel_src} -> {sym_dst}: {e}")


def main(cmake_file: str, file_extension: str, root_path: str, subdir: str):
    rel_include_path = os.path.join("Sources", root_path)
    include_path = os.path.join(os.path.abspath(os.getcwd()), rel_include_path)

    cmake_dir = os.path.dirname(cmake_file)
    headers_by_set = extract_headers_by_set(cmake_file, file_extension)

    for set_name, headers in headers_by_set.items():
        normalized_headers = normalize_paths(headers, cmake_dir)
        if len(normalized_headers) > 0:
            create_subdirectories(set_name, include_path)
            header_path = os.path.join(rel_include_path, set_name)
            if len(subdir) > 0:
                header_path = os.path.join(header_path, subdir)
            create_symlinks(headers, header_path, cmake_dir)


if __name__ == "__main__":
    if len(sys.argv) != 5:
        print(
            "Usage: python3 extract_headers_by_set.py <path/to/CMakeLists.txt> <file extension> <root dir>"
        )
        sys.exit(1)

    cmake_file = sys.argv[1]
    file_extension = sys.argv[2]
    root_dir = sys.argv[3]
    sub_dir = sys.argv[4]
    main(cmake_file, file_extension, root_dir, sub_dir)
