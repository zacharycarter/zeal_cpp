#include "device/boot_config.h"
#include "config.h"
#include "core/containers/map.h"
#include "core/json/json_object.h"
#include "core/json/sjson.h"
#include "core/memory/temp_allocator.h"
#include "core/platform.h"
#include "core/strings/dynamic_string.h"

namespace zeal
{
BootConfig::BootConfig(Allocator &a)
    : boot_script_name(u64(0)), boot_package_name(u64(0)), window_title(a),
      window_w(ZEAL_DEFAULT_WINDOW_WIDTH), window_h(ZEAL_DEFAULT_WINDOW_HEIGHT),
      aspect_ratio(-1.0f), vsync(true), fullscreen(false) {}

bool BootConfig::parse(const char *json)
{
    TempAllocator4096 ta;
    JsonObject cfg(ta);
    sjson::parse(json, cfg);

    // General configs
    boot_script_name = sjson::parse_resource_id(cfg["boot_script"]);
    boot_package_name = sjson::parse_resource_id(cfg["boot_package"]);

    if (json_object::has(cfg, "window_title"))
        sjson::parse_string(cfg["window_title"], window_title);

    // Platform-specific configs
    if (json_object::has(cfg, ZEAL_PLATFORM_NAME))
    {
        JsonObject platform(ta);
        sjson::parse(cfg[ZEAL_PLATFORM_NAME], platform);

        if (json_object::has(platform, "renderer"))
        {
            JsonObject renderer(ta);
            sjson::parse(platform["renderer"], renderer);

            if (json_object::has(renderer, "resolution"))
            {
                JsonArray resolution(ta);
                sjson::parse_array(renderer["resolution"], resolution);
                window_w = sjson::parse_int(resolution[0]);
                window_h = sjson::parse_int(resolution[1]);
            }
            if (json_object::has(renderer, "aspect_ratio"))
                aspect_ratio = sjson::parse_float(renderer["aspect_ratio"]);
            if (json_object::has(renderer, "vsync"))
                vsync = sjson::parse_bool(renderer["vsync"]);
            if (json_object::has(renderer, "fullscreen"))
                fullscreen = sjson::parse_bool(renderer["fullscreen"]);
        }
    }

    return true;
}

} // namespace zeal