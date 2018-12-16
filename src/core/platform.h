#ifndef ZEAL_PLATFORM_H_HEADER_GUARD
#define ZEAL_PLATFORM_H_HEADER_GUARD

// Architecture
#define ZEAL_ARCH_32BIT 0
#define ZEAL_ARCH_64BIT 0

// Compiler
#define ZEAL_COMPILER_CLANG          0
#define ZEAL_COMPILER_CLANG_ANALYZER 0
#define ZEAL_COMPILER_GCC            0
#define ZEAL_COMPILER_MSVC           0

// Endianess
#define ZEAL_CPU_ENDIAN_BIG    0
#define ZEAL_CPU_ENDIAN_LITTLE 0

// CPU
#define ZEAL_CPU_ARM   0
#define ZEAL_CPU_JIT   0
#define ZEAL_CPU_MIPS  0
#define ZEAL_CPU_PPC   0
#define ZEAL_CPU_RISCV 0
#define ZEAL_CPU_X86   0

// C Runtime
#define ZEAL_CRT_BIONIC 0
#define ZEAL_CRT_GLIBC  0
#define ZEAL_CRT_LIBCXX 0
#define ZEAL_CRT_MINGW  0
#define ZEAL_CRT_MSVC   0
#define ZEAL_CRT_NEWLIB 0

#ifndef ZEAL_CRT_MUSL
#	define ZEAL_CRT_MUSL 0
#endif // ZEAL_CRT_MUSL

#ifndef ZEAL_CRT_NONE
#	define ZEAL_CRT_NONE 0
#endif // ZEAL_CRT_NONE

// Platform
#define ZEAL_PLATFORM_ANDROID    0
#define ZEAL_PLATFORM_BSD        0
#define ZEAL_PLATFORM_EMSCRIPTEN 0
#define ZEAL_PLATFORM_HURD       0
#define ZEAL_PLATFORM_IOS        0
#define ZEAL_PLATFORM_LINUX      0
#define ZEAL_PLATFORM_NX         0
#define ZEAL_PLATFORM_OSX        0
#define ZEAL_PLATFORM_PS4        0
#define ZEAL_PLATFORM_RPI        0
#define ZEAL_PLATFORM_STEAMLINK  0
#define ZEAL_PLATFORM_WINDOWS    0
#define ZEAL_PLATFORM_WINRT      0
#define ZEAL_PLATFORM_XBOXONE    0

// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Compilers
#if defined(__clang__)
// clang defines __GNUC__ or _MSC_VER
#	undef  ZEAL_COMPILER_CLANG
#	define ZEAL_COMPILER_CLANG (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
#	if defined(__clang_analyzer__)
#		undef  ZEAL_COMPILER_CLANG_ANALYZER
#		define ZEAL_COMPILER_CLANG_ANALYZER 1
#	endif // defined(__clang_analyzer__)
#elif defined(_MSC_VER)
#	undef  ZEAL_COMPILER_MSVC
#	define ZEAL_COMPILER_MSVC _MSC_VER
#elif defined(__GNUC__)
#	undef  ZEAL_COMPILER_GCC
#	define ZEAL_COMPILER_GCC (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
#	error "ZEAL_COMPILER_* is not defined!"
#endif //

// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Architectures
#if defined(__arm__)     \
 || defined(__aarch64__) \
 || defined(_M_ARM)
#	undef  ZEAL_CPU_ARM
#	define ZEAL_CPU_ARM 1
#	define ZEAL_CACHE_LINE_SIZE 64
#elif defined(__MIPSEL__)     \
 ||   defined(__mips_isa_rev) \
 ||   defined(__mips64)
#	undef  ZEAL_CPU_MIPS
#	define ZEAL_CPU_MIPS 1
#	define ZEAL_CACHE_LINE_SIZE 64
#elif defined(_M_PPC)        \
 ||   defined(__powerpc__)   \
 ||   defined(__powerpc64__)
#	undef  ZEAL_CPU_PPC
#	define ZEAL_CPU_PPC 1
#	define ZEAL_CACHE_LINE_SIZE 128
#elif defined(__riscv)   \
 ||   defined(__riscv__) \
 ||   defined(RISCVEL)
#	undef  ZEAL_CPU_RISCV
#	define ZEAL_CPU_RISCV 1
#	define ZEAL_CACHE_LINE_SIZE 64
#elif defined(_M_IX86)    \
 ||   defined(_M_X64)     \
 ||   defined(__i386__)   \
 ||   defined(__x86_64__)
#	undef  ZEAL_CPU_X86
#	define ZEAL_CPU_X86 1
#	define ZEAL_CACHE_LINE_SIZE 64
#else // PNaCl doesn't have CPU defined.
#	undef  ZEAL_CPU_JIT
#	define ZEAL_CPU_JIT 1
#	define ZEAL_CACHE_LINE_SIZE 64
#endif //

#if defined(__x86_64__)    \
 || defined(_M_X64)        \
 || defined(__aarch64__)   \
 || defined(__64BIT__)     \
 || defined(__mips64)      \
 || defined(__powerpc64__) \
 || defined(__ppc64__)     \
 || defined(__LP64__)
#	undef  ZEAL_ARCH_64BIT
#	define ZEAL_ARCH_64BIT 64
#else
#	undef  ZEAL_ARCH_32BIT
#	define ZEAL_ARCH_32BIT 32
#endif //

#if ZEAL_CPU_PPC
#	undef  ZEAL_CPU_ENDIAN_BIG
#	define ZEAL_CPU_ENDIAN_BIG 1
#else
#	undef  ZEAL_CPU_ENDIAN_LITTLE
#	define ZEAL_CPU_ENDIAN_LITTLE 1
#endif // ZEAL_PLATFORM_

// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Operating_Systems
#if defined(_DURANGO) || defined(_XBOX_ONE)
#	undef  ZEAL_PLATFORM_XBOXONE
#	define ZEAL_PLATFORM_XBOXONE 1
#elif defined(_WIN32) || defined(_WIN64)
// http://msdn.microsoft.com/en-us/library/6sehtctf.aspx
#	ifndef NOMINMAX
#		define NOMINMAX
#	endif // NOMINMAX
//  If _USING_V110_SDK71_ is defined it means we are using the v110_xp or v120_xp toolset.
#	if defined(_MSC_VER) && (_MSC_VER >= 1700) && (!_USING_V110_SDK71_)
#		include <winapifamily.h>
#	endif // defined(_MSC_VER) && (_MSC_VER >= 1700) && (!_USING_V110_SDK71_)
#	if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
#		undef  ZEAL_PLATFORM_WINDOWS
#		if !defined(WINVER) && !defined(_WIN32_WINNT)
#			if ZEAL_ARCH_64BIT
//				When building 64-bit target Win7 and above.
#				define WINVER 0x0601
#				define _WIN32_WINNT 0x0601
#			else
//				Windows Server 2003 with SP1, Windows XP with SP2 and above
#				define WINVER 0x0502
#				define _WIN32_WINNT 0x0502
#			endif // ZEAL_ARCH_64BIT
#		endif // !defined(WINVER) && !defined(_WIN32_WINNT)
#		define ZEAL_PLATFORM_WINDOWS _WIN32_WINNT
#	else
#		undef  ZEAL_PLATFORM_WINRT
#		define ZEAL_PLATFORM_WINRT 1
#	endif
#elif defined(__ANDROID__)
// Android compiler defines __linux__
#	include <sys/cdefs.h> // Defines __BIONIC__ and includes android/api-level.h
#	undef  ZEAL_PLATFORM_ANDROID
#	define ZEAL_PLATFORM_ANDROID __ANDROID_API__
#elif defined(__STEAMLINK__)
// SteamLink compiler defines __linux__
#	undef  ZEAL_PLATFORM_STEAMLINK
#	define ZEAL_PLATFORM_STEAMLINK 1
#elif defined(__VCCOREVER__)
// RaspberryPi compiler defines __linux__
#	undef  ZEAL_PLATFORM_RPI
#	define ZEAL_PLATFORM_RPI 1
#elif  defined(__linux__)
#	undef  ZEAL_PLATFORM_LINUX
#	define ZEAL_PLATFORM_LINUX 1
#elif  defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) \
	|| defined(__ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__)
#	undef  ZEAL_PLATFORM_IOS
#	define ZEAL_PLATFORM_IOS 1
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
#	undef  ZEAL_PLATFORM_OSX
#	define ZEAL_PLATFORM_OSX __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#elif defined(__EMSCRIPTEN__)
#	undef  ZEAL_PLATFORM_EMSCRIPTEN
#	define ZEAL_PLATFORM_EMSCRIPTEN 1
#elif defined(__ORBIS__)
#	undef  ZEAL_PLATFORM_PS4
#	define ZEAL_PLATFORM_PS4 1
#elif  defined(__FreeBSD__)        \
	|| defined(__FreeBSD_kernel__) \
	|| defined(__NetBSD__)         \
	|| defined(__OpenBSD__)        \
	|| defined(__DragonFly__)
#	undef  ZEAL_PLATFORM_BSD
#	define ZEAL_PLATFORM_BSD 1
#elif defined(__GNU__)
#	undef  ZEAL_PLATFORM_HURD
#	define ZEAL_PLATFORM_HURD 1
#elif defined(__NX__)
#	undef  ZEAL_PLATFORM_NX
#	define ZEAL_PLATFORM_NX 1
#endif //

#if !ZEAL_CRT_NONE
// https://sourceforge.net/p/predef/wiki/Libraries/
#	if defined(__BIONIC__)
#		undef  ZEAL_CRT_BIONIC
#		define ZEAL_CRT_BIONIC 1
#	elif defined(_MSC_VER)
#		undef  ZEAL_CRT_MSVC
#		define ZEAL_CRT_MSVC 1
#	elif defined(__GLIBC__)
#		undef  ZEAL_CRT_GLIBC
#		define ZEAL_CRT_GLIBC (__GLIBC__ * 10000 + __GLIBC_MINOR__ * 100)
#	elif defined(__MINGW32__) || defined(__MINGW64__)
#		undef  ZEAL_CRT_MINGW
#		define ZEAL_CRT_MINGW 1
#	elif defined(__apple_build_version__) || defined(__ORBIS__) || defined(__EMSCRIPTEN__) || defined(__llvm__)
#		undef  ZEAL_CRT_LIBCXX
#		define ZEAL_CRT_LIBCXX 1
#	endif //

#	if !ZEAL_CRT_BIONIC \
	&& !ZEAL_CRT_GLIBC  \
	&& !ZEAL_CRT_LIBCXX \
	&& !ZEAL_CRT_MINGW  \
	&& !ZEAL_CRT_MSVC   \
	&& !ZEAL_CRT_MUSL   \
	&& !ZEAL_CRT_NEWLIB
#		undef  ZEAL_CRT_NONE
#		define ZEAL_CRT_NONE 1
#	endif // ZEAL_CRT_*
#endif // !ZEAL_CRT_NONE

#define ZEAL_PLATFORM_POSIX (0   \
	||  ZEAL_PLATFORM_ANDROID    \
	||  ZEAL_PLATFORM_BSD        \
	||  ZEAL_PLATFORM_EMSCRIPTEN \
	||  ZEAL_PLATFORM_HURD       \
	||  ZEAL_PLATFORM_IOS        \
	||  ZEAL_PLATFORM_LINUX      \
	||  ZEAL_PLATFORM_NX         \
	||  ZEAL_PLATFORM_OSX        \
	||  ZEAL_PLATFORM_PS4        \
	||  ZEAL_PLATFORM_RPI        \
	||  ZEAL_PLATFORM_STEAMLINK  \
	)

#define ZEAL_PLATFORM_NONE !(0   \
	||  ZEAL_PLATFORM_ANDROID    \
	||  ZEAL_PLATFORM_BSD        \
	||  ZEAL_PLATFORM_EMSCRIPTEN \
	||  ZEAL_PLATFORM_HURD       \
	||  ZEAL_PLATFORM_IOS        \
	||  ZEAL_PLATFORM_LINUX      \
	||  ZEAL_PLATFORM_NX         \
	||  ZEAL_PLATFORM_OSX        \
	||  ZEAL_PLATFORM_PS4        \
	||  ZEAL_PLATFORM_RPI        \
	||  ZEAL_PLATFORM_STEAMLINK  \
	||  ZEAL_PLATFORM_WINDOWS    \
	||  ZEAL_PLATFORM_WINRT      \
	||  ZEAL_PLATFORM_XBOXONE    \
	)

#if ZEAL_COMPILER_GCC
#	define ZEAL_COMPILER_NAME "GCC "       \
		ZEAL_STRINGIZE(__GNUC__) "."       \
		ZEAL_STRINGIZE(__GNUC_MINOR__) "." \
		ZEAL_STRINGIZE(__GNUC_PATCHLEVEL__)
#elif ZEAL_COMPILER_CLANG
#	define ZEAL_COMPILER_NAME "Clang "      \
		ZEAL_STRINGIZE(__clang_major__) "." \
		ZEAL_STRINGIZE(__clang_minor__) "." \
		ZEAL_STRINGIZE(__clang_patchlevel__)
#elif ZEAL_COMPILER_MSVC
#	if ZEAL_COMPILER_MSVC >= 1910 // Visual Studio 2017
#		define ZEAL_COMPILER_NAME "MSVC 15.0"
#	elif ZEAL_COMPILER_MSVC >= 1900 // Visual Studio 2015
#		define ZEAL_COMPILER_NAME "MSVC 14.0"
#	elif ZEAL_COMPILER_MSVC >= 1800 // Visual Studio 2013
#		define ZEAL_COMPILER_NAME "MSVC 12.0"
#	elif ZEAL_COMPILER_MSVC >= 1700 // Visual Studio 2012
#		define ZEAL_COMPILER_NAME "MSVC 11.0"
#	elif ZEAL_COMPILER_MSVC >= 1600 // Visual Studio 2010
#		define ZEAL_COMPILER_NAME "MSVC 10.0"
#	elif ZEAL_COMPILER_MSVC >= 1500 // Visual Studio 2008
#		define ZEAL_COMPILER_NAME "MSVC 9.0"
#	else
#		define ZEAL_COMPILER_NAME "MSVC"
#	endif //
#endif // ZEAL_COMPILER_

#if ZEAL_PLATFORM_ANDROID
#	define ZEAL_PLATFORM_NAME "Android " \
				ZEAL_STRINGIZE(ZEAL_PLATFORM_ANDROID)
#elif ZEAL_PLATFORM_BSD
#	define ZEAL_PLATFORM_NAME "BSD"
#elif ZEAL_PLATFORM_EMSCRIPTEN
#	define ZEAL_PLATFORM_NAME "asm.js "          \
		ZEAL_STRINGIZE(__EMSCRIPTEN_major__) "." \
		ZEAL_STRINGIZE(__EMSCRIPTEN_minor__) "." \
		ZEAL_STRINGIZE(__EMSCRIPTEN_tiny__)
#elif ZEAL_PLATFORM_HURD
#	define ZEAL_PLATFORM_NAME "Hurd"
#elif ZEAL_PLATFORM_IOS
#	define ZEAL_PLATFORM_NAME "iOS"
#elif ZEAL_PLATFORM_LINUX
#	define ZEAL_PLATFORM_NAME "Linux"
#elif ZEAL_PLATFORM_NONE
#	define ZEAL_PLATFORM_NAME "None"
#elif ZEAL_PLATFORM_NX
#	define ZEAL_PLATFORM_NAME "NX"
#elif ZEAL_PLATFORM_OSX
#	define ZEAL_PLATFORM_NAME "OSX"
#elif ZEAL_PLATFORM_PS4
#	define ZEAL_PLATFORM_NAME "PlayStation 4"
#elif ZEAL_PLATFORM_RPI
#	define ZEAL_PLATFORM_NAME "RaspberryPi"
#elif ZEAL_PLATFORM_STEAMLINK
#	define ZEAL_PLATFORM_NAME "SteamLink"
#elif ZEAL_PLATFORM_WINDOWS
#	define ZEAL_PLATFORM_NAME "Windows"
#elif ZEAL_PLATFORM_WINRT
#	define ZEAL_PLATFORM_NAME "WinRT"
#elif ZEAL_PLATFORM_XBOXONE
#	define ZEAL_PLATFORM_NAME "Xbox One"
#else
#	error "Unknown ZEAL_PLATFORM!"
#endif // ZEAL_PLATFORM_

#if ZEAL_CPU_ARM
#	define ZEAL_CPU_NAME "ARM"
#elif ZEAL_CPU_JIT
#	define ZEAL_CPU_NAME "JIT-VM"
#elif ZEAL_CPU_MIPS
#	define ZEAL_CPU_NAME "MIPS"
#elif ZEAL_CPU_PPC
#	define ZEAL_CPU_NAME "PowerPC"
#elif ZEAL_CPU_RISCV
#	define ZEAL_CPU_NAME "RISC-V"
#elif ZEAL_CPU_X86
#	define ZEAL_CPU_NAME "x86"
#endif // ZEAL_CPU_

#if ZEAL_CRT_BIONIC
#	define ZEAL_CRT_NAME "Bionic libc"
#elif ZEAL_CRT_GLIBC
#	define ZEAL_CRT_NAME "GNU C Library"
#elif ZEAL_CRT_MSVC
#	define ZEAL_CRT_NAME "MSVC C Runtime"
#elif ZEAL_CRT_MINGW
#	define ZEAL_CRT_NAME "MinGW C Runtime"
#elif ZEAL_CRT_LIBCXX
#	define ZEAL_CRT_NAME "Clang C Library"
#elif ZEAL_CRT_NEWLIB
#	define ZEAL_CRT_NAME "Newlib"
#elif ZEAL_CRT_MUSL
#	define ZEAL_CRT_NAME "musl libc"
#elif ZEAL_CRT_NONE
#	define ZEAL_CRT_NAME "None"
#else
#	error "Unknown ZEAL_CRT!"
#endif // ZEAL_CRT_

#if ZEAL_ARCH_32BIT
#	define ZEAL_ARCH_NAME "32-bit"
#elif ZEAL_ARCH_64BIT
#	define ZEAL_ARCH_NAME "64-bit"
#endif // ZEAL_ARCH_

#endif // ZEAL_PLATFORM_H_HEADER_GUARD