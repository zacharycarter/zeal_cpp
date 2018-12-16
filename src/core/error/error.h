#ifndef ZEAL_ERROR_H_HEADER_GUARD
#define ZEAL_ERROR_H_HEADER_GUARD

#include "core/types.h"

/// @defgroup Error Error
/// @ingroup Core
namespace zeal
{
/// Error management.
///
/// @ingroup Error
namespace error
{
	/// Aborts the program execution logging an error message and the stacktrace if
	/// the platform supports it.
    void abort(const char* format, ...);

} // namespace error

} // namespace zeal

#if ZEAL_DEBUG
	#define ZE_ASSERT(condition, msg, ...)                   \
		do                                                   \
		{                                                    \
			if (ZE_UNLIKELY(!(condition)))                   \
			{                                                \
				zeal::error::abort("Assertion failed: %s\n"  \
					"    In: %s:%d\n"                        \
					"    " msg "\n"                          \
					, # condition                            \
					, __FILE__                               \
					, __LINE__                               \
					, ## __VA_ARGS__                         \
					);                                       \
				ZE_UNREACHABLE();                            \
			}                                                \
		} while (0)
#else
	#define ZE_ASSERT(...) ZE_NOOP()
#endif // ZEAL_DEBUG

#define ZE_FATAL(msg, ...) ZE_ASSERT(false, msg, ## __VA_ARGS__)
#define ZE_ENSURE(condition) ZE_ASSERT(condition, "")

#endif // ZEAL_ERROR_H_HEADER_GUARD