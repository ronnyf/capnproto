//
//  KJFoundation.h
//  KJFoundation
//
//  Created by Ronny Falk on 7/22/24.
//

#import <Foundation/Foundation.h>

//! Project version number for KJFoundation.
FOUNDATION_EXPORT double KJFoundationVersionNumber;

//! Project version string for KJFoundation.
FOUNDATION_EXPORT const unsigned char KJFoundationVersionString[];

// In this header, you should import all the public headers of your framework using statements like #import <KJFoundation/PublicHeader.h>

#include <kj/arena.h>
#include <kj/array.h>
#include <kj/cidr.h>
#include <kj/common.h>
#include <kj/debug.h>
#include <kj/encoding.h>
#include <kj/exception.h>
#include <kj/filesystem.h>
#include <kj/function.h>
#include <kj/hash.h>
#include <kj/io.h>
#include <kj/list.h>
#include <kj/main.h>
#include <kj/map.h>
#include <kj/memory.h>
#include <kj/miniposix.h>
#include <kj/mutex.h>
#include <kj/one-of.h>
#include <kj/parse_char.h>
#include <kj/parse_common.h>
#include <kj/refcount.h>
#include <kj/source-location.h>
//#include <kj/iostream.hpp>
#include <kj/string.hpp>
#include <kj/string-tree.h>
#include <kj/table.h>
#include <kj/test.h>
#include <kj/thread.h>
#include <kj/threadlocal.h>
#include <kj/time.hpp>
#include <kj/tuple.h>
#include <kj/units.h>
#include <kj/vector.h>
#include <kj/windows-sanity.h>

// async:
#include <kj/async.h>
#include <kj/async-inl.h>
#include <kj/async-io.h>
#include <kj/async-io-internal.h>
#include <kj/async-prelude.h>
#include <kj/async-queue.h>
#include <kj/async-unix.h>
#include <kj/timer.h>

// compat:
#include <kj/readiness-io.h>
#include <kj/tls.h>
#include <kj/http.h>
#include <kj/url.h>
#include <kj/gzip.h>
#include <kj/brotli.h>
