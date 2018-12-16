#include "device/device.h"
#include "config.h"
#include "core/containers/array.h"
#include "core/fs/path.h"
#include "core/memory/memory.h"
#include "core/memory/temp_allocator.h"
#include "core/os.h"
#include "core/strings/string.h"
#include "core/strings/string_stream.h"
#include "core/types.h"
#include "device/console_server.h"
#include "device/log.h"

#define MAX_SUBSYSTEMS_HEAP 8 * 1024 * 1024

LOG_SYSTEM(DEVICE, "device")

namespace zeal
{
Device::Device(const DeviceOptions &opts, ConsoleServer &cs)
	: _allocator(default_allocator(), MAX_SUBSYSTEMS_HEAP),
	  _device_options(opts), _console_server(&cs), _width(0), _height(0),
	  _quit(false), _paused(false)
{
}

void Device::log(const char *msg)
{
	if (_last_log)
	{
		_last_log->write(msg, strlen32(msg));
		_last_log->write("\n", 1);
		_last_log->flush();
	}
}

char _buffer[sizeof(Device)];
Device *_device = NULL;

void run(const DeviceOptions &opts)
{
	ZE_ASSERT(_device == NULL, "Crown already initialized");
	console_server_globals::init();
	_device = new (_buffer) Device(opts, *console_server());
	_device->run();
	_device->~Device();
	_device = NULL;
	console_server_globals::shutdown();
}

Device *device() { return zeal::_device; }

} // namespace zeal