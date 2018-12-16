#ifndef ZEAL_TYPES_H_HEADER_GUARD
#define ZEAL_TYPES_H_HEADER_GUARD

#include "core/platform.h"
#include <stdint.h>

#ifndef ZEAL_DEBUG
	#define ZEAL_DEBUG 0
#endif // ZEAL_DEBUG

#ifndef ZEAL_DEVELOPMENT
	#define ZEAL_DEVELOPMENT 0
#endif // ZEAL_DEVELOPMENT

/// @defgroup Core Core

namespace zeal
{
/// @addtogroup Core
/// @{
typedef int8_t   s8;
typedef uint8_t  u8;
typedef int16_t  s16;
typedef uint16_t u16;
typedef int32_t  s32;
typedef uint32_t u32;
typedef int64_t  s64;
typedef uint64_t u64;
typedef float    f32;
typedef double   f64;
/// @}

} // namespace zeal

#ifndef NULL
	#define NULL 0
#endif

#define ZE_NOOP(...) do { (void)0; } while (0)
#define ZE_UNUSED(x) do { (void)(x); } while (0)
#define ZE_STATIC_ASSERT(condition, ...) static_assert(condition, "" # __VA_ARGS__)

#if ZEAL_PLATFORM_LINUX || ZEAL_PLATFORM_OSX
	#define ZE_ALIGN_DECL(align, decl) decl __attribute__ ((aligned (align)))
#elif ZEAL_PLATFORM_WINDOWS
	#define ZE_ALIGN_DECL(align_, decl) __declspec (align(align_)) decl
#endif

#endif // ZEAL_TYPES_H_HEADER_GUARD