// Copyright (c) 2013-2017 Sandstorm Development Group, Inc. and contributors
// Licensed under the MIT License:
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "capnp/compiler/type-id.h"
#include <kj/debug.h>
#include <string.h>

namespace capnp {
namespace compiler {

class TypeIdGenerator {
  // A non-cryptographic deterministic random number generator used to generate type IDs when the
  // developer did not specify one themselves.
  //
  // The underlying algorithm is MD5. MD5 is safe to use here because this is not intended to be a
  // cryptographic random number generator. In retrospect it would have been nice to use something
  // else just to avoid people freaking out about it, but changing the algorithm now would break
  // backwards-compatibility.

public:
  TypeIdGenerator();

  void update(kj::ArrayPtr<const kj::byte> data);
  inline void update(kj::ArrayPtr<const char> data) {
    return update(data.asBytes());
  }
  inline void update(kj::StringPtr data) {
    return update(data.asArray());
  }

  kj::ArrayPtr<const kj::byte> finish();

private:
  bool finished = false;

  struct {
    uint lo, hi;
    uint a, b, c, d;
    kj::byte buffer[64];
    uint block[16];
  } ctx;

  const kj::byte* body(const kj::byte* ptr, size_t size);
};

uint64_t generateChildId(uint64_t parentId, kj::StringPtr childName) {
  // Compute ID by hashing the concatenation of the parent ID and the declaration name, and
  // then taking the first 8 bytes.

  kj::byte parentIdBytes[sizeof(uint64_t)];
  for (uint i = 0; i < sizeof(uint64_t); i++) {
    parentIdBytes[i] = (parentId >> (i * 8)) & 0xff;
  }

  TypeIdGenerator generator;
  generator.update(kj::arrayPtr(parentIdBytes, kj::size(parentIdBytes)));
  generator.update(childName);

  kj::ArrayPtr<const kj::byte> resultBytes = generator.finish();

  uint64_t result = 0;
  for (uint i = 0; i < sizeof(uint64_t); i++) {
    result = (result << 8) | resultBytes[i];
  }

  return result | (1ull << 63);
}

uint64_t generateGroupId(uint64_t parentId, uint16_t groupIndex) {
  // Compute ID by hashing the concatenation of the parent ID and the group index, and
  // then taking the first 8 bytes.

  kj::byte bytes[sizeof(uint64_t) + sizeof(uint16_t)];
  for (uint i = 0; i < sizeof(uint64_t); i++) {
    bytes[i] = (parentId >> (i * 8)) & 0xff;
  }
  for (uint i = 0; i < sizeof(uint16_t); i++) {
    bytes[sizeof(uint64_t) + i] = (groupIndex >> (i * 8)) & 0xff;
  }

  TypeIdGenerator generator;
  generator.update(bytes);

  kj::ArrayPtr<const kj::byte> resultBytes = generator.finish();

  uint64_t result = 0;
  for (uint i = 0; i < sizeof(uint64_t); i++) {
    result = (result << 8) | resultBytes[i];
  }

  return result | (1ull << 63);
}

uint64_t generateMethodParamsId(uint64_t parentId, uint16_t methodOrdinal, bool isResults) {
  // Compute ID by hashing the concatenation of the parent ID, the method ordinal, and a
  // boolean indicating whether this is the params or the results, and then taking the first 8
  // bytes.

  kj::byte bytes[sizeof(uint64_t) + sizeof(uint16_t) + 1];
  for (uint i = 0; i < sizeof(uint64_t); i++) {
    bytes[i] = (parentId >> (i * 8)) & 0xff;
  }
  for (uint i = 0; i < sizeof(uint16_t); i++) {
    bytes[sizeof(uint64_t) + i] = (methodOrdinal >> (i * 8)) & 0xff;
  }
  bytes[sizeof(bytes) - 1] = isResults;

  TypeIdGenerator generator;
  generator.update(bytes);

  kj::ArrayPtr<const kj::byte> resultBytes = generator.finish();

  uint64_t result = 0;
  for (uint i = 0; i < sizeof(uint64_t); i++) {
    result = (result << 8) | resultBytes[i];
  }

  return result | (1ull << 63);
}

// The remainder of this file was derived from code placed in the public domain.
// The original code bore the following notice:

/*
 * This is an OpenSSL-compatible implementation of the RSA Data Security, Inc.
 * MD5 Message-Digest Algorithm (RFC 1321).
 *
 * Homepage:
 * http://openwall.info/wiki/people/solar/software/public-domain-source-code/md5
 *
 * Author:
 * Alexander Peslyak, better known as Solar Designer <solar at openwall.com>
 *
 * This software was written by Alexander Peslyak in 2001.  No copyright is
 * claimed, and the software is hereby placed in the public domain.
 * In case this attempt to disclaim copyright and place the software in the
 * public domain is deemed null and void, then the software is
 * Copyright (c) 2001 Alexander Peslyak and it is hereby released to the
 * general public under the following terms:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * There's ABSOLUTELY NO WARRANTY, express or implied.
 *
 * (This is a heavily cut-down "BSD license".)
 *
 * This differs from Colin Plumb's older public domain implementation in that
 * no exactly 32-bit integer data type is required (any 32-bit or wider
 * unsigned integer data type will do), there's no compile-time endianness
 * configuration, and the function prototypes match OpenSSL's.  No code from
 * Colin Plumb's implementation has been reused; this comment merely compares
 * the properties of the two independent implementations.
 *
 * The primary goals of this implementation are portability and ease of use.
 * It is meant to be fast, but not as fast as possible.  Some known
 * optimizations are not included to reduce source code size and avoid
 * compile-time configuration.
 */

/*
 * The basic MD5 functions.
 *
 * F and G are optimized compared to their RFC 1321 definitions for
 * architectures that lack an AND-NOT instruction, just like in Colin Plumb's
 * implementation.
 */
#define F(x, y, z)      ((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z)      ((y) ^ ((z) & ((x) ^ (y))))
#define H(x, y, z)      ((x) ^ (y) ^ (z))
#define I(x, y, z)      ((y) ^ ((x) | ~(z)))

/*
 * The MD5 transformation for all four rounds.
 */
#define STEP(f, a, b, c, d, x, t, s) \
  (a) += f((b), (c), (d)) + (x) + (t); \
  (a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s)))); \
  (a) += (b);

/*
 * SET reads 4 input bytes in little-endian byte order and stores them
 * in a properly aligned word in host byte order.
 *
 * The check for little-endian architectures that tolerate unaligned
 * memory accesses is just an optimization.  Nothing will break if it
 * doesn't work.
 */
#if defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#define SET(n) \
  (*(uint *)&ptr[(n) * 4])
#define GET(n) \
  SET(n)
#else
#define SET(n) \
  (ctx.block[(n)] = \
  (uint)ptr[(n) * 4] | \
  ((uint)ptr[(n) * 4 + 1] << 8) | \
  ((uint)ptr[(n) * 4 + 2] << 16) | \
  ((uint)ptr[(n) * 4 + 3] << 24))
#define GET(n) \
  (ctx.block[(n)])
#endif

/*
 * This processes one or more 64-byte data blocks, but does NOT update
 * the bit counters.  There are no alignment requirements.
 */
const kj::byte* TypeIdGenerator::body(const kj::byte* ptr, size_t size)
{
  uint a, b, c, d;
  uint saved_a, saved_b, saved_c, saved_d;

  a = ctx.a;
  b = ctx.b;
  c = ctx.c;
  d = ctx.d;

  do {
    saved_a = a;
    saved_b = b;
    saved_c = c;
    saved_d = d;

/* Round 1 */
    STEP(F, a, b, c, d, SET(0), 0xd76aa478, 7)
    STEP(F, d, a, b, c, SET(1), 0xe8c7b756, 12)
    STEP(F, c, d, a, b, SET(2), 0x242070db, 17)
    STEP(F, b, c, d, a, SET(3), 0xc1bdceee, 22)
    STEP(F, a, b, c, d, SET(4), 0xf57c0faf, 7)
    STEP(F, d, a, b, c, SET(5), 0x4787c62a, 12)
    STEP(F, c, d, a, b, SET(6), 0xa8304613, 17)
    STEP(F, b, c, d, a, SET(7), 0xfd469501, 22)
    STEP(F, a, b, c, d, SET(8), 0x698098d8, 7)
    STEP(F, d, a, b, c, SET(9), 0x8b44f7af, 12)
    STEP(F, c, d, a, b, SET(10), 0xffff5bb1, 17)
    STEP(F, b, c, d, a, SET(11), 0x895cd7be, 22)
    STEP(F, a, b, c, d, SET(12), 0x6b901122, 7)
    STEP(F, d, a, b, c, SET(13), 0xfd987193, 12)
    STEP(F, c, d, a, b, SET(14), 0xa679438e, 17)
    STEP(F, b, c, d, a, SET(15), 0x49b40821, 22)

/* Round 2 */
    STEP(G, a, b, c, d, GET(1), 0xf61e2562, 5)
    STEP(G, d, a, b, c, GET(6), 0xc040b340, 9)
    STEP(G, c, d, a, b, GET(11), 0x265e5a51, 14)
    STEP(G, b, c, d, a, GET(0), 0xe9b6c7aa, 20)
    STEP(G, a, b, c, d, GET(5), 0xd62f105d, 5)
    STEP(G, d, a, b, c, GET(10), 0x02441453, 9)
    STEP(G, c, d, a, b, GET(15), 0xd8a1e681, 14)
    STEP(G, b, c, d, a, GET(4), 0xe7d3fbc8, 20)
    STEP(G, a, b, c, d, GET(9), 0x21e1cde6, 5)
    STEP(G, d, a, b, c, GET(14), 0xc33707d6, 9)
    STEP(G, c, d, a, b, GET(3), 0xf4d50d87, 14)
    STEP(G, b, c, d, a, GET(8), 0x455a14ed, 20)
    STEP(G, a, b, c, d, GET(13), 0xa9e3e905, 5)
    STEP(G, d, a, b, c, GET(2), 0xfcefa3f8, 9)
    STEP(G, c, d, a, b, GET(7), 0x676f02d9, 14)
    STEP(G, b, c, d, a, GET(12), 0x8d2a4c8a, 20)

/* Round 3 */
    STEP(H, a, b, c, d, GET(5), 0xfffa3942, 4)
    STEP(H, d, a, b, c, GET(8), 0x8771f681, 11)
    STEP(H, c, d, a, b, GET(11), 0x6d9d6122, 16)
    STEP(H, b, c, d, a, GET(14), 0xfde5380c, 23)
    STEP(H, a, b, c, d, GET(1), 0xa4beea44, 4)
    STEP(H, d, a, b, c, GET(4), 0x4bdecfa9, 11)
    STEP(H, c, d, a, b, GET(7), 0xf6bb4b60, 16)
    STEP(H, b, c, d, a, GET(10), 0xbebfbc70, 23)
    STEP(H, a, b, c, d, GET(13), 0x289b7ec6, 4)
    STEP(H, d, a, b, c, GET(0), 0xeaa127fa, 11)
    STEP(H, c, d, a, b, GET(3), 0xd4ef3085, 16)
    STEP(H, b, c, d, a, GET(6), 0x04881d05, 23)
    STEP(H, a, b, c, d, GET(9), 0xd9d4d039, 4)
    STEP(H, d, a, b, c, GET(12), 0xe6db99e5, 11)
    STEP(H, c, d, a, b, GET(15), 0x1fa27cf8, 16)
    STEP(H, b, c, d, a, GET(2), 0xc4ac5665, 23)

/* Round 4 */
    STEP(I, a, b, c, d, GET(0), 0xf4292244, 6)
    STEP(I, d, a, b, c, GET(7), 0x432aff97, 10)
    STEP(I, c, d, a, b, GET(14), 0xab9423a7, 15)
    STEP(I, b, c, d, a, GET(5), 0xfc93a039, 21)
    STEP(I, a, b, c, d, GET(12), 0x655b59c3, 6)
    STEP(I, d, a, b, c, GET(3), 0x8f0ccc92, 10)
    STEP(I, c, d, a, b, GET(10), 0xffeff47d, 15)
    STEP(I, b, c, d, a, GET(1), 0x85845dd1, 21)
    STEP(I, a, b, c, d, GET(8), 0x6fa87e4f, 6)
    STEP(I, d, a, b, c, GET(15), 0xfe2ce6e0, 10)
    STEP(I, c, d, a, b, GET(6), 0xa3014314, 15)
    STEP(I, b, c, d, a, GET(13), 0x4e0811a1, 21)
    STEP(I, a, b, c, d, GET(4), 0xf7537e82, 6)
    STEP(I, d, a, b, c, GET(11), 0xbd3af235, 10)
    STEP(I, c, d, a, b, GET(2), 0x2ad7d2bb, 15)
    STEP(I, b, c, d, a, GET(9), 0xeb86d391, 21)

    a += saved_a;
    b += saved_b;
    c += saved_c;
    d += saved_d;

    ptr += 64;
  } while (size -= 64);

  ctx.a = a;
  ctx.b = b;
  ctx.c = c;
  ctx.d = d;

  return ptr;
}

TypeIdGenerator::TypeIdGenerator()
{
  ctx.a = 0x67452301;
  ctx.b = 0xefcdab89;
  ctx.c = 0x98badcfe;
  ctx.d = 0x10325476;

  ctx.lo = 0;
  ctx.hi = 0;
}

void TypeIdGenerator::update(kj::ArrayPtr<const kj::byte> dataArray)
{
  KJ_REQUIRE(!finished, "already called TypeIdGenerator::finish()");

  const kj::byte* data = dataArray.begin();
  unsigned long size = dataArray.size();

  uint saved_lo;
  unsigned long used, free;

  saved_lo = ctx.lo;
  if ((ctx.lo = (saved_lo + size) & 0x1fffffff) < saved_lo)
    ctx.hi++;
  ctx.hi += size >> 29;

  used = saved_lo & 0x3f;

  if (used) {
    free = 64 - used;

    if (size < free) {
      memcpy(&ctx.buffer[used], data, size);
      return;
    }

    memcpy(&ctx.buffer[used], data, free);
    data = data + free;
    size -= free;
    body(ctx.buffer, 64);
  }

  if (size >= 64) {
    data = body(data, size & ~(unsigned long)0x3f);
    size &= 0x3f;
  }

  memcpy(ctx.buffer, data, size);
}

kj::ArrayPtr<const kj::byte> TypeIdGenerator::finish()
{
  if (!finished) {
    unsigned long used, free;

    used = ctx.lo & 0x3f;

    ctx.buffer[used++] = 0x80;

    free = 64 - used;

    if (free < 8) {
      memset(&ctx.buffer[used], 0, free);
      body(ctx.buffer, 64);
      used = 0;
      free = 64;
    }

    memset(&ctx.buffer[used], 0, free - 8);

    ctx.lo <<= 3;
    ctx.buffer[56] = ctx.lo;
    ctx.buffer[57] = ctx.lo >> 8;
    ctx.buffer[58] = ctx.lo >> 16;
    ctx.buffer[59] = ctx.lo >> 24;
    ctx.buffer[60] = ctx.hi;
    ctx.buffer[61] = ctx.hi >> 8;
    ctx.buffer[62] = ctx.hi >> 16;
    ctx.buffer[63] = ctx.hi >> 24;

    body(ctx.buffer, 64);

    // Store final result into ctx.buffer.
    ctx.buffer[0] = ctx.a;
    ctx.buffer[1] = ctx.a >> 8;
    ctx.buffer[2] = ctx.a >> 16;
    ctx.buffer[3] = ctx.a >> 24;
    ctx.buffer[4] = ctx.b;
    ctx.buffer[5] = ctx.b >> 8;
    ctx.buffer[6] = ctx.b >> 16;
    ctx.buffer[7] = ctx.b >> 24;
    ctx.buffer[8] = ctx.c;
    ctx.buffer[9] = ctx.c >> 8;
    ctx.buffer[10] = ctx.c >> 16;
    ctx.buffer[11] = ctx.c >> 24;
    ctx.buffer[12] = ctx.d;
    ctx.buffer[13] = ctx.d >> 8;
    ctx.buffer[14] = ctx.d >> 16;
    ctx.buffer[15] = ctx.d >> 24;

    finished = true;
  }

  return kj::arrayPtr(ctx.buffer, 16);
}


}  // namespace compiler
}  // namespace capnp
