#include "config.h"
#include "core/memory/memory.h"
#include "device/device.h"

#define MAX_SUBSYSTEMS_HEAP 8 * 1024 * 1024

namespace zeal
{


char _buffer[sizeof(Device)];
Device* _device = NULL;

Device* device()
{
	return zeal::_device;
}

} // namespace zeal