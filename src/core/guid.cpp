#include "core/guid.h"
#include "core/platform.h"
#include "core/strings/dynamic_string.h"

#if ZEAL_PLATFORM_POSIX
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#elif ZEAL_PLATFORM_WINDOWS
#include <objbase.h>
#endif // ZEAL_PLATFORM_POSIX

namespace zeal
{
namespace guid
{
Guid new_guid()
{
    Guid guid;
#if ZEAL_PLATFORM_POSIX
    int fd = open("/dev/urandom", O_RDONLY);
    ZE_ASSERT(fd != -1, "open: errno = %d", errno);
    ssize_t rb = read(fd, &guid, sizeof(guid));
    ZE_ENSURE(rb == sizeof(guid));
    ZE_UNUSED(rb);
    close(fd);
    guid.data3 = (guid.data3 & 0x4fffu) | 0x4000u;
    guid.data4 = (guid.data4 & 0x3fffffffffffffffu) | 0x8000000000000000u;
#elif ZEAL_PLATFORM_WINDOWS
    HRESULT hr = CoCreateGuid((GUID *)&guid);
    ZE_ASSERT(hr == S_OK, "CoCreateGuid: error");
    ZE_UNUSED(hr);
#endif // ZEAL_PLATFORM_POSIX
    return guid;
}

Guid parse(const char *str)
{
    Guid guid;
    try_parse(guid, str);
    return guid;
}

bool try_parse(Guid &guid, const char *str)
{
    ZE_ENSURE(NULL != str);
    u32 a, b, c, d, e, f;
    int num = sscanf(str, "%8x-%4x-%4x-%4x-%4x%8x", &a, &b, &c, &d, &e, &f);
    guid.data1 = a;
    guid.data2 = (u16)(b & 0x0000ffffu);
    guid.data3 = (u16)(c & 0x0000ffffu);
    guid.data4 =
        (u64)(d & 0x0000ffffu) << 48 | (u64)(e & 0x0000ffffu) << 32 | (u64)f;
    return num == 6;
}

void to_string(const Guid &guid, DynamicString &str)
{
    char buf[36 + 1];
    snprintf(buf, sizeof(buf), "%.8x-%.4x-%.4x-%.4x-%.4x%.8x", guid.data1,
             guid.data2, guid.data3,
             (u16)((guid.data4 & 0xffff000000000000u) >> 48),
             (u16)((guid.data4 & 0x0000ffff00000000u) >> 32),
             (u32)((guid.data4 & 0x00000000ffffffffu) >> 0));
    str.set(buf, sizeof(buf) - 1);
}

} // namespace guid

} // namespace zeal