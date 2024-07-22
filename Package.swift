// swift-tools-version: 5.9
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "capnproto",
    products: [
        .library(
            name: "kj",
            targets: ["kj"]),
        .library(
            name: "capnp",
            targets: ["capnp-heavy"]),
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
            name: "kj",
            path: "Sources",
            sources: [
                "kj/arena.c++",
                "kj/array.c++",
                "kj/async-io-unix.c++",
                "kj/async-io-win32.c++",
                "kj/async-io.c++",
                "kj/async-unix.c++",
                "kj/async-win32.c++",
                "kj/async.c++",
                "kj/cidr.c++",
                "kj/common.c++",
                "kj/compat/brotli.c++",
                "kj/compat/gzip.c++",
                "kj/compat/http.c++",
                "kj/compat/readiness-io.c++",
                "kj/compat/tls.c++",
                "kj/compat/url.c++",
                "kj/debug.c++",
                "kj/encoding.c++",
                "kj/exception.c++",
                "kj/filesystem-disk-unix.c++",
                "kj/filesystem-disk-win32.c++",
                "kj/filesystem.c++",
                "kj/hash.c++",
                "kj/io.c++",
                "kj/list.c++",
                "kj/kmain.c++",
                "kj/memory.c++",
                "kj/mutex.c++",
                "kj/parse/char.c++",
                "kj/refcount.c++",
                "kj/source-location.c++",
                "kj/string-tree.c++",
                "kj/string.c++",
                "kj/table.c++",
                "kj/thread.c++",
                "kj/time.c++",
                "kj/timer.c++",
                "kj/units.c++",
            ],
            publicHeadersPath: "kj-include",
            cSettings: [
            ],
            linkerSettings: [
                .linkedLibrary("c++"),
                .linkedLibrary("pthread"),
            ]
        ),
        .target(
            name: "capnp-heavy",
            dependencies: ["kj"],
            path: "Sources",
            sources: [
                "capnp/any.c++",
                "capnp/arena.c++",
                "capnp/blob.c++",
                "capnp/c++.capnp.c++",
                "capnp/capability.c++",
                "capnp/dynamic-capability.c++",
                "capnp/dynamic.c++",
                "capnp/ez-rpc.c++",
                "capnp/layout.c++",
                "capnp/list.c++",
                "capnp/membrane.c++",
                "capnp/message.c++",
                "capnp/persistent.capnp.c++",
                "capnp/reconnect.c++",
                "capnp/rpc-twoparty.c++",
                "capnp/rpc-twoparty.capnp.c++",
                "capnp/rpc.c++",
                "capnp/rpc.capnp.c++",
                "capnp/schema-loader.c++",
                "capnp/schema-parser.c++",
                "capnp/schema.c++",
                "capnp/schema.capnp.c++",
                "capnp/serialize-async.c++",
                "capnp/serialize-packed.c++",
                "capnp/serialize-text.c++",
                "capnp/serialize.c++",
                "capnp/stream.capnp.c++",
                "capnp/stringify.c++",
                // compat:
                "capnp/compat/json.c++",
                "capnp/compat/json.capnp.c++",
                "capnp/compat/websocket-rpc.c++",
                // compiler:
                "capnp/compiler/compiler.c++",
                "capnp/compiler/error-reporter.c++",
                "capnp/compiler/generics.c++",
                "capnp/compiler/grammar.capnp.c++",
                "capnp/compiler/lexer.c++",
                "capnp/compiler/lexer.capnp.c++",
                "capnp/compiler/module-loader.c++",
                "capnp/compiler/node-translator.c++",
                "capnp/compiler/parser.c++",
                "capnp/compiler/type-id.c++",
            ],
            publicHeadersPath: "include"
        ),
        .executableTarget(
            name: "capnp-tool",
            dependencies: ["capnp-heavy", "kj"],
            path: "Sources",
            sources: [
                "capnp/compiler/capnp.c++"
            ],
            publicHeadersPath: "capnpc-cpp-include"
        ),
        .executableTarget(
            name: "capnpc-c++",
            dependencies: ["capnp-heavy", "kj"],
            path: "Sources",
            sources: [
                "capnp/compiler/capnpc-c++.c++"
            ],
            publicHeadersPath: "capnpc-cpp-include"
        ),
        .executableTarget(
            name: "capnpc-capnp",
            dependencies: ["capnp-heavy", "kj"],
            path: "Sources",
            sources: [
                "capnp/compiler/capnpc-capnp.c++"
            ],
            publicHeadersPath: "capnpc-cpp-include"
        )
    ],
    cLanguageStandard: .c17,
    cxxLanguageStandard: .gnucxx20
)
