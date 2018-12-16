#ifndef ZEAL_DEVICE_OPTIONS_H_HEADER_GUARD
#define ZEAL_DEVICE_OPTIONS_H_HEADER_GUARD

#include "core/platform.h"
#include "core/strings/dynamic_string.h"
#include "core/types.h"

namespace zeal
{
/// Holds device options.
///
/// @ingroup Device
struct DeviceOptions
{
    int _argc;
    const char **_argv;
    DynamicString _source_dir;
    const char *_map_source_dir_name;
    DynamicString _map_source_dir_prefix;
    DynamicString _data_dir;
    const char *_boot_dir;
    const char *_platform;
    DynamicString _lua_string;
    bool _wait_console;
    bool _do_compile;
    bool _do_continue;
    bool _server;
    u32 _parent_window;
    u16 _console_port;
    u16 _window_x;
    u16 _window_y;
    u16 _window_width;
    u16 _window_height;

    DeviceOptions(Allocator &a, int argc, const char **argv);

    /// Parses the command line and returns
    /// EXIT_SUCCESS if no error is found.
    int parse(bool *quit);
};

} // namespace zeal

#endif // ZEAL_DEVICE_OPTIONS_H_HEADER_GUARD