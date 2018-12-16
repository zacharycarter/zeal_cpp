#ifndef ZEAL_DEVICE_H_HEADER_GUARD
#define ZEAL_DEVICE_H_HEADER_GUARD

#include "config.h"
#include "core/containers/types.h"
#include "core/fs/types.h"
#include "core/memory/allocator.h"
#include "core/memory/linear_allocator.h"
#include "core/strings/string_id.h"
#include "core/types.h"
#include "device/device_options.h"

/// @defgroup Device Device
namespace zeal
{
/// This is the place to look for accessing all of
/// the engine subsystems and related stuff.
///
/// @ingroup Device
struct Device
{
    LinearAllocator _allocator;

    const DeviceOptions &_device_options;

    u16 _width;
    u16 _height;

    bool _quit;
    bool _paused;

    bool process_events(bool vsync);

    /// Runs the engine.
    void run();

    /// Returns the number of command line parameters.
    int argc() const { return _device_options._argc; }

    /// Returns command line parameters.
    const char **argv() const { return (const char **)_device_options._argv; }

    /// Quits the application.
    void quit();

    /// Pauses the engine.
    void pause();

    /// Unpauses the engine.
    void unpause();

    /// Returns the main window resolution.
    void resolution(u16 &width, u16 &height);
};

/// Runs the engine.
void run(const DeviceOptions &opts);

} // namespace zeal

#endif // ZEAL_DEVICE_H_HEADER_GUARD
