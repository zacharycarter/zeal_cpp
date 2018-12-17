#include "device/device.h"
#include "config.h"
#include "core/containers/array.h"
#include "core/fs/file.h"
#include "core/fs/filesystem.h"
#include "core/fs/filesystem_disk.h"
#include "core/fs/path.h"
#include "core/json/json_object.h"
#include "core/json/sjson.h"
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
	  _device_options(opts), _console_server(&cs), _lua_environment(NULL),
	  _width(0), _height(0), _quit(false), _paused(false) {}

static void console_command_script(ConsoleServer & /*cs*/, TCPSocket /*client*/,
								   const char *json, void *user_data)
{
	TempAllocator4096 ta;
	JsonObject obj(ta);
	DynamicString script(ta);

	sjson::parse(json, obj);
	sjson::parse_string(obj["script"], script);

	((Device *)user_data)->_lua_environment->execute_string(script.c_str());
}

static void console_command(ConsoleServer &cs, TCPSocket client,
							const char *json, void *user_data)
{
	TempAllocator4096 ta;
	JsonObject obj(ta);
	JsonArray args(ta);

	sjson::parse(json, obj);
	sjson::parse_array(obj["args"], args);

	DynamicString cmd(ta);
	sjson::parse_string(args[0], cmd);

	if (cmd == "pause")
		device()->pause();
	else if (cmd == "unpause")
		device()->unpause();
	else if (cmd == "reload")
	{
		if (array::size(args) != 3)
		{
			cs.error(client, "Usage: reload type name");
			return;
		}

		DynamicString type(ta);
		DynamicString name(ta);
		sjson::parse_string(args[1], type);
		sjson::parse_string(args[2], name);

		((Device *)user_data)
			->reload(ResourceId(type.c_str()), ResourceId(name.c_str()));
	}
}

void Device::run()
{
	_console_server->register_command("command", console_command, this);
	_console_server->register_command("script", console_command_script, this);

	_console_server->listen(_device_options._console_port,
							_device_options._wait_console);
}

void Device::reload(StringId64 type, StringId64 name)
{
	StringId64 mix;
	mix._id = type._id ^ name._id;

	TempAllocator128 ta;
	DynamicString path(ta);
	mix.to_string(path);

	logi(DEVICE, "Reloading #ID(%s)", path.c_str());

	_resource_manager->reload(type, name);
	const void *new_resource = _resource_manager->get(type, name);

	if (type == RESOURCE_TYPE_SCRIPT)
	{
		_lua_environment->execute((const LuaResource *)new_resource);
	}

	logi(DEVICE, "Reloaded #ID(%s)", path.c_str());
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