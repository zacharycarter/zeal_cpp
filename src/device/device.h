#ifndef ZEAL_DEVICE_H_HEADER_GUARD
#define ZEAL_DEVICE_H_HEADER_GUARD

#include "config.h"
#include "core/memory/allocator.h"
#include "core/memory/linear_allocator.h"
#include "core/types.h"

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
};

} // namespace zeal

#endif // ZEAL_DEVICE_H_HEADER_GUARD
