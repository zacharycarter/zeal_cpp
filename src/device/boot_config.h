#ifndef ZEAL_BOOT_CONFIG_H_HEADER_GUARD
#define ZEAL_BOOT_CONFIG_H_HEADER_GUARD

#include "core/strings/dynamic_string.h"
#include "core/strings/string_id.h"
#include "core/types.h"

namespace zeal
{
/// Boot configuration.
///
/// @ingroup Device
struct BootConfig
{
    StringId64 boot_script_name;
    StringId64 boot_package_name;
    DynamicString window_title;
    u16 window_w;
    u16 window_h;
    float aspect_ratio;
    bool vsync;
    bool fullscreen;

    BootConfig(Allocator &a);
    bool parse(const char *json);
};

} // namespace zeal

#endif // ZEAL_BOOT_CONFIG_H_HEADER_GUARD