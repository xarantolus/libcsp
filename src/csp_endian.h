/* Byte-order conversion macros (htobe16 ... le64toh) for libcsp sources.
 *
 * The <endian.h> header providing these is a glibc/musl extension that
 * macOS doesn't ship. There, map the macros onto libkern's OSByteOrder;
 * everywhere else this is exactly the system <endian.h>. */
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

#include <endian.h>

#endif
