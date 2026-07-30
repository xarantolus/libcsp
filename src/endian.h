/* <endian.h> is glibc/musl-only, so libcsp sources including it don't compile
 * on macOS. This shim sits on the build's `-I src` include path: on macOS it
 * maps the glibc byte-order macros onto libkern's, everywhere else it defers
 * to the real system header via #include_next, so Linux builds see the exact
 * same header they always did. */
#pragma once

#if defined(__APPLE__)

#include <libkern/OSByteOrder.h>
#include <machine/endian.h>

#define htobe16(x) OSSwapHostToBigInt16(x)
#define htole16(x) OSSwapHostToLittleInt16(x)
#define be16toh(x) OSSwapBigToHostInt16(x)
#define le16toh(x) OSSwapLittleToHostInt16(x)

#define htobe32(x) OSSwapHostToBigInt32(x)
#define htole32(x) OSSwapHostToLittleInt32(x)
#define be32toh(x) OSSwapBigToHostInt32(x)
#define le32toh(x) OSSwapLittleToHostInt32(x)

#define htobe64(x) OSSwapHostToBigInt64(x)
#define htole64(x) OSSwapHostToLittleInt64(x)
#define be64toh(x) OSSwapBigToHostInt64(x)
#define le64toh(x) OSSwapLittleToHostInt64(x)

#ifndef __BYTE_ORDER
#define __BYTE_ORDER BYTE_ORDER
#define __BIG_ENDIAN BIG_ENDIAN
#define __LITTLE_ENDIAN LITTLE_ENDIAN
#endif

#else /* !__APPLE__ */

#include_next <endian.h>

#endif
