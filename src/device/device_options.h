#ifndef ZEAL_DEVICE_OPTIONS_H_HEADER_GUARD
#define ZEAL_DEVICE_OPTIONS_H_HEADER_GUARD

#include "core/platform.h"
#include "core/types.h"

namespace zeal
{
/// Holds device options.
///
/// @ingroup Device
struct DeviceOptions
{
    DeviceOptions(Allocator& a, int argc, const char** argv);

    /// Parses the command line and returns
	/// EXIT_SUCCESS if no error is found.
    int parse(bool* quit);
};

} // namespace zeal

#endif // ZEAL_DEVICE_OPTIONS_H_HEADER_GUARD