#include "core/time.h"

#if ZEAL_PLATFORM_POSIX
#include <time.h> // clock_gettime
#elif ZEAL_PLATFORM_WINDOWS
#include <windows.h>
#endif

#if ZEAL_PLATFORM_OSX
#include <mach/clock.h>
#include <mach/mach.h>
#endif

namespace zeal
{
namespace time
{
s64 now()
{
#if ZEAL_PLATFORM_LINUX || ZEAL_PLATFORM_ANDROID
    timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec * s64(1000000000) + now.tv_nsec;
#elif ZEAL_PLATFORM_OSX
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    return mts.tv_sec * s64(1000000000) + mts.tv_nsec;
#elif ZEAL_PLATFORM_WINDOWS
    LARGE_INTEGER ttime;
    QueryPerformanceCounter(&ttime);
    return (s64)ttime.QuadPart;
#endif
}

inline s64 frequency()
{
#if ZEAL_PLATFORM_LINUX || ZEAL_PLATFORM_ANDROID
    return s64(1000000000);
#elif ZEAL_PLATFORM_OSX
    return s64(1000000);
#elif ZEAL_PLATFORM_WINDOWS
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return (s64)freq.QuadPart;
#endif
}

f64 seconds(s64 ticks) { return f64(ticks) / f64(frequency()); }

} // namespace time

} // namespace zeal