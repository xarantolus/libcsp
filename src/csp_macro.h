#pragma once

#include "csp/autoconfig.h"

#if (CSP_ZEPHYR)
#include <zephyr/kernel.h>
#else
/* Mach-O has no plain ".noinit" section (a section specifier needs
 * "segment,section"), so builds on macOS fail to compile. __noinit is only a
 * skip-zero-initialization optimization for embedded RAM; plain
 * zero-initialized static variables are a safe superset on a hosted OS. */
#if defined(__APPLE__)
#define __noinit
#else
#define __noinit __attribute__((section(".noinit")))
#endif
#define __packed __attribute__((__packed__))
#define __maybe_unused __attribute__((__unused__))
#define __unused __attribute__((__unused__))
#ifdef __CYGWIN__
#define __weak
#else
#if defined(__APPLE__)
/* Apple clang predefines __weak as an Objective-C ownership qualifier (a
 * no-op in plain C). Weak symbol definitions are still supported on Mach-O,
 * so replace the built-in macro with the usual weak-symbol attribute. */
#undef __weak
#endif
#define __weak   __attribute__((__weak__))
#endif

#define CONTAINER_OF(ptr, type, member) \
	((type *)(void *)((char *)(ptr) - offsetof(type, member)))

#endif
