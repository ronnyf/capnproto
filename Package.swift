// swift-tools-version: 5.9
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "capnproto",
    products: [
        .library(
            name: "kj",
            targets: ["kj-lite", "kj-heavy", "kj-async", "kj-http", "kj-tls", "kj-gzip"]),
        .library(
            name: "capnp",
            targets: ["capnp-lite", "capnp-heavy", "capnp-rpc", "capnp-json"]),
        .library(
            name: "capnp-lite",
            targets: ["capnp-lite"]),
        .library(
            name: "capnpc",
            targets: ["capnpc"]),
        .executable(
            name: "capnp-tool",
            targets: ["capnp-tool"]),
        .executable(
            name: "capnpc-c++",
            targets: ["capnpc-c++"]),
        .executable(
            name: "capnpc-capnp",
            targets: ["capnpc-capnp"]),
    ],
    targets: [
        .target(
            name: "kj-lite",
            path: "Sources",
            sources: [
                "kj/array.c++",
                "kj/cidr.c++",
                "kj/list.c++",
                "kj/common.c++",
                "kj/debug.c++",
                "kj/exception.c++",
                "kj/io.c++",
                "kj/memory.c++",
                "kj/mutex.c++",
                "kj/string.c++",
                "kj/source-location.c++",
                "kj/hash.c++",
                "kj/table.c++",
                "kj/thread.c++",
                "kj/kmain.c++",
                "kj/arena.c++",
                "kj/test-helpers.c++",
                "kj/units.c++",
                "kj/encoding.c++",
            ],
            publicHeadersPath: "include"
        ),
        .target(
            name: "kj-heavy",
            dependencies: ["kj-lite"],
            path: "Sources",
            sources: [
                "kj/refcount.c++",
                "kj/string-tree.c++",
                "kj/time.c++",
                "kj/filesystem.c++",
                "kj/filesystem-disk-unix.c++",
                "kj/filesystem-disk-win32.c++",
                "kj/parse/char.c++",
            ],
            publicHeadersPath: "include"
        ),
        .target(
            name: "kj-async",
            dependencies: ["kj-heavy"],
            path: "Sources",
            sources: [
                "kj/async.c++",
                "kj/async-unix.c++",
                "kj/async-win32.c++",
                "kj/async-io-win32.c++",
                "kj/async-io.c++",
                "kj/async-io-unix.c++",
                "kj/timer.c++",
            ],
            publicHeadersPath: "include"
        ),
        .target(
            name: "kj-http",
            dependencies: ["kj-async"],
            path: "Sources",
            sources: [
                "kj/compat/url.c++",
                "kj/compat/http.c++",
            ],
            publicHeadersPath: "include"
        ),
        .target(
            name: "kj-tls",
            dependencies: ["kj-async"],
            path: "Sources",
            sources: [
                "kj/compat/readiness-io.c++",
                "kj/compat/tls.c++",
            ],
            publicHeadersPath: "include"
        ),
        .target(
            name: "kj-gzip",
            dependencies: ["kj-async"],
            path: "Sources",
            sources: [
                "kj/compat/gzip.c++",
            ],
            publicHeadersPath: "include"
        ),
        .target(
            name: "capnp-lite",
            dependencies: ["kj-lite"],
            path: "Sources",
            sources: [
                "capnp/c++.capnp.c++",
                "capnp/blob.c++",
                "capnp/arena.c++",
                "capnp/layout.c++",
                "capnp/list.c++",
                "capnp/any.c++",
                "capnp/message.c++",
                "capnp/schema.capnp.c++",
                "capnp/stream.capnp.c++",
                "capnp/serialize.c++",
                "capnp/serialize-packed.c++",
            ],
            publicHeadersPath: "include"
        ),
        .target(
            name: "capnp-heavy",
            dependencies: ["capnp-lite"],
            path: "Sources",
            sources: [
                "capnp/schema.c++",
                "capnp/schema-loader.c++",
                "capnp/dynamic.c++",
                "capnp/stringify.c++",
            ],
            publicHeadersPath: "include"
        ),
        .target(
            name: "capnp-rpc",
            dependencies: ["kj-async", "capnp-lite"],
            path: "Sources",
            sources: [
                "capnp/serialize-async.c++",
                "capnp/capability.c++",
                "capnp/membrane.c++",
                "capnp/dynamic-capability.c++",
                "capnp/rpc.c++",
                "capnp/rpc.capnp.c++",
                "capnp/rpc-twoparty.c++",
                "capnp/rpc-twoparty.capnp.c++",
                "capnp/persistent.capnp.c++",
                "capnp/ez-rpc.c++",
            ],
            publicHeadersPath: "include"
        ),
        .target(
            name: "capnp-json",
            dependencies: ["capnp-heavy"],
            path: "Sources",
            sources: [
                "capnp/compat/json.c++",
                "capnp/compat/json.capnp.c++",
            ],
            publicHeadersPath: "include"
        ),
        .target(
            name: "capnp-websocket",
            dependencies: ["capnp-rpc"],
            path: "Sources",
            sources: [
                "capnp/compat/websocket-rpc.c++",
            ],
            publicHeadersPath: "include"
        ),
        .target(
            name: "capnpc",
            dependencies: ["capnp-lite"],
            path: "Sources",
            sources: [
                "capnp/compiler/type-id.c++",
                "capnp/compiler/error-reporter.c++",
                "capnp/compiler/lexer.capnp.c++",
                "capnp/compiler/lexer.c++",
                "capnp/compiler/grammar.capnp.c++",
                "capnp/compiler/parser.c++",
                "capnp/compiler/generics.c++",
                "capnp/compiler/node-translator.c++",
                "capnp/compiler/compiler.c++",
                "capnp/schema-parser.c++",
                "capnp/serialize-text.c++",
            ],
            publicHeadersPath: "include"
        ),
        .executableTarget(
            name: "capnp-tool",
            dependencies: ["capnpc", "capnp-json", "kj-heavy"],
            path: "Sources",
            sources: [
                "capnp/compiler/module-loader.c++",
                "capnp/compiler/capnp.c++",
            ],
            publicHeadersPath: "include"
        ),
        .executableTarget(
            name: "capnpc-c++",
            dependencies: ["capnpc", "capnp-json", "kj-heavy"],
            path: "Sources",
            sources: [
                "capnp/compiler/capnpc-c++.c++",
            ],
            publicHeadersPath: "include"
        ),
        .executableTarget(
            name: "capnpc-capnp",
            dependencies: ["capnpc", "capnp-json", "kj-heavy"],
            path: "Sources",
            sources: [
                "capnp/compiler/capnpc-capnp.c++",
            ],
            publicHeadersPath: "include"
        )
    ],
    cLanguageStandard: .c17,
    cxxLanguageStandard: .gnucxx20
)
