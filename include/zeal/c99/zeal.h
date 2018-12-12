#ifndef ZEAL_C99_H_HEADER_GUARD
#define ZEAL_C99_H_HEADER_GUARD

#include <stdbool.h> // bool

#ifndef ZEAL_SHARED_LIB_BUILD
#    define ZEAL_SHARED_LIB_BUILD 0
#endif // ZEAL_SHARED_LIB_BUILD

#if ZEAL_PLATFORM_WINDOWS
#   define ZEAL_SYMBOL_EXPORT __declspec(dllexport)
#   define ZEAL_SYMBOL_IMPORT __declspec(dllimport)
#else
#   define ZEAL_SYMBOL_EXPORT __attribute__((visibility("default")))
#   define ZEAL_SYMBOL_IMPORT
#endif // ZEAL_PLATFORM_WINDOWS

#if ZEAL_SHARED_LIB_BUILD
#   define ZEAL_SHARED_LIB_API ZEAL_SYMBOL_EXPORT
#else
#   define ZEAL_SHARED_LIB_API
#endif // ZEAL_SHARED_LIB_*

#if defined(__cplusplus)
#   define ZEAL_C_API extern "C" ZEAL_SHARED_LIB_API
#else
#   define ZEAL_C_API ZEAL_SHARED_LIB_API
#endif // defined(__cplusplus)

ZEAL_C_API bool zeal_init();

#endif // ZEAL_C99_H_HEADER_GUARD