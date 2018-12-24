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
#include "device/profiler.h"
#include "lua/lua_environment.h"
#include "resource/config_resource.h"
#include "resource/resource_loader.h"
#include "resource/resource_manager.h"
#include <bgfx/bgfx.h>
#include <bx/allocator.h>

#define MAX_SUBSYSTEMS_HEAP 8 * 1024 * 1024

LOG_SYSTEM(DEVICE, "device")

namespace zeal
{
struct BgfxCallback : public bgfx::CallbackI
{
	virtual void fatal(const char *_filePath, uint16_t _line,
					   bgfx::Fatal::Enum _code, const char *_str)
	{
		ZE_ASSERT(false, "Fatal error: 0x%08x: %s", _code, _str);
		ZE_UNUSED(_filePath);
		ZE_UNUSED(_line);
		ZE_UNUSED(_code);
		ZE_UNUSED(_str);
	}

	virtual void traceVargs(const char * /*_filePath*/, u16 /*_line*/,
							const char *_format, va_list _argList)
	{
		char buf[2048];
		strncpy(buf, _format, sizeof(buf) - 1);
		buf[strlen32(buf) - 1] = '\0'; // Remove trailing newline
		vlogi(DEVICE, buf, _argList);
	}

	virtual void profilerBegin(const char * /*_name*/, uint32_t /*_abgr*/,
							   const char * /*_filePath*/, uint16_t /*_line*/) {}

	virtual void profilerBeginLiteral(const char * /*_name*/, uint32_t /*_abgr*/,
									  const char * /*_filePath*/,
									  uint16_t /*_line*/) {}

	virtual void profilerEnd() {}

	virtual u32 cacheReadSize(u64 /*_id*/) { return 0; }

	virtual bool cacheRead(u64 /*_id*/, void * /*_data*/, u32 /*_size*/)
	{
		return false;
	}

	virtual void cacheWrite(u64 /*_id*/, const void * /*_data*/, u32 /*_size*/) {}

	virtual void screenShot(const char * /*_filePath*/, u32 /*_width*/,
							u32 /*_height*/, u32 /*_pitch*/,
							const void * /*_data*/, u32 /*_size*/,
							bool /*_yflip*/) {}

	virtual void captureBegin(u32 /*_width*/, u32 /*_height*/, u32 /*_pitch*/,
							  bgfx::TextureFormat::Enum /*_format*/,
							  bool /*_yflip*/) {}

	virtual void captureEnd() {}

	virtual void captureFrame(const void * /*_data*/, u32 /*_size*/) {}
};

struct BgfxAllocator : public bx::AllocatorI
{
	ProxyAllocator _allocator;

	BgfxAllocator(Allocator &a) : _allocator(a, "bgfx") {}

	virtual void *realloc(void *_ptr, size_t _size, size_t _align,
						  const char * /*_file*/, u32 /*_line*/)
	{
		if (!_ptr)
			return _allocator.allocate((u32)_size,
									   (u32)_align == 0 ? 16 : (u32)_align);

		if (_size == 0)
		{
			_allocator.deallocate(_ptr);
			return NULL;
		}

		// Realloc
		void *p =
			_allocator.allocate((u32)_size, (u32)_align == 0 ? 16 : (u32)_align);
		_allocator.deallocate(_ptr);
		return p;
	}
};

Device::Device(const DeviceOptions &opts, ConsoleServer &cs)
	: _allocator(default_allocator(), MAX_SUBSYSTEMS_HEAP),
	  _device_options(opts), _boot_config(default_allocator()),
	  _console_server(&cs), _data_filesystem(NULL), _last_log(NULL),
	  _resource_loader(NULL), _resource_manager(NULL), _bgfx_allocator(NULL),
	  _bgfx_callback(NULL), _lua_environment(NULL), _display(NULL),
	  _window(NULL), _width(0), _height(0), _quit(false), _paused(false) {}

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

	_data_filesystem = ZE_NEW(_allocator, FilesystemDisk)(default_allocator());
	{
		char cwd[1024];
		const char *data_dir = !_device_options._data_dir.empty()
								   ? _device_options._data_dir.c_str()
								   : os::getcwd(cwd, sizeof(cwd));
		((FilesystemDisk *)_data_filesystem)->set_prefix(data_dir);
	}

	_last_log = _data_filesystem->open(ZEAL_LAST_LOG, FileOpenMode::WRITE);

	logi(DEVICE, "Initializing Zeal Engine %s %s %s", ZEAL_VERSION,
		 ZEAL_PLATFORM_NAME, ZEAL_ARCH_NAME);

	profiler_globals::init();

	namespace cor = config_resource_internal;

	_resource_loader = ZE_NEW(_allocator, ResourceLoader)(*_data_filesystem);
	_resource_loader->register_fallback(RESOURCE_TYPE_TEXTURE,
										StringId64("core/fallback/fallback"));
	_resource_loader->register_fallback(RESOURCE_TYPE_MATERIAL,
										StringId64("core/fallback/fallback"));
	_resource_loader->register_fallback(RESOURCE_TYPE_UNIT,
										StringId64("core/fallback/fallback"));

	_resource_manager = ZE_NEW(_allocator, ResourceManager)(*_resource_loader);
	_resource_manager->register_type(RESOURCE_TYPE_CONFIG,
									 RESOURCE_VERSION_CONFIG, cor::load,
									 cor::unload, NULL, NULL);

	// Read config
	{
		TempAllocator512 ta;
		DynamicString boot_dir(ta);

		if (_device_options._boot_dir != NULL)
		{
			boot_dir += _device_options._boot_dir;
			boot_dir += '/';
		}
		boot_dir += ZEAL_BOOT_CONFIG;

		const StringId64 config_name(boot_dir.c_str());
		_resource_manager->load(RESOURCE_TYPE_CONFIG, config_name);
		_resource_manager->flush();
		_boot_config.parse((const char *)_resource_manager->get(
			RESOURCE_TYPE_CONFIG, config_name));
		_resource_manager->unload(RESOURCE_TYPE_CONFIG, config_name);
	}

	_bgfx_allocator = ZE_NEW(_allocator, BgfxAllocator)(default_allocator());
	_bgfx_callback = ZE_NEW(_allocator, BgfxCallback)();

	_width = _boot_config.window_w;
	_height = _boot_config.window_h;

	_window = window::create(_allocator);
	_window->open(_device_options._window_x, _device_options._window_y, _width,
				  _height, _device_options._parent_window);
	_window->set_title(_boot_config.window_title.c_str());
	_window->set_fullscreen(_boot_config.fullscreen);
	_window->bgfx_setup();

	bgfx::Init init;
	init.type = bgfx::RendererType::Count;
	init.vendorId = BGFX_PCI_ID_NONE;
	init.resolution.width = _width;
	init.resolution.height = _height;
	init.resolution.reset =
		_boot_config.vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE;
	init.callback = _bgfx_callback;
	init.allocator = _bgfx_allocator;
	bgfx::init(init);

	while (!_quit)
	{
		// const s64 time = time::now();
		// const f32 dt = time::seconds(time - time_last);
		// time_last = time;

		// profiler_globals::clear();
		// _console_server->update();
	}

	_lua_environment->call_global("shutdown", 0);

	ZE_DELETE(_allocator, _lua_environment);
	ZE_DELETE(_allocator, _resource_manager);
	ZE_DELETE(_allocator, _resource_loader);

	bgfx::shutdown();
	_window->close();
	window::destroy(_allocator, *_window);
	display::destroy(_allocator, *_display);
	ZE_DELETE(_allocator, _bgfx_callback);
	ZE_DELETE(_allocator, _bgfx_allocator);

	if (_last_log)
		_data_filesystem->close(*_last_log);

	ZE_DELETE(_allocator, _data_filesystem);

	profiler_globals::shutdown();

	_allocator.clear();
}

void Device::pause()
{
	_paused = true;
	logi(DEVICE, "Paused");
}

void Device::unpause()
{
	_paused = false;
	logi(DEVICE, "Unpaused");
}

void Device::reload(StringId64 type, StringId64 name)
{
	StringId64 mix;
	mix._id = type._id ^ name._id;

	TempAllocator128 ta;
	DynamicString path(ta);
	mix.to_string(path);

	// logi(DEVICE, "Reloading #ID(%s)", path.c_str());

	// _resource_manager->reload(type, name);
	// const void *new_resource = _resource_manager->get(type, name);

	// if (type == RESOURCE_TYPE_SCRIPT)
	// {
	// 	_lua_environment->execute((const LuaResource *)new_resource);
	// }

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
	ZE_ASSERT(_device == NULL, "Zeal already initialized");
	console_server_globals::init();
	_device = new (_buffer) Device(opts, *console_server());
	_device->run();
	_device->~Device();
	_device = NULL;
	console_server_globals::shutdown();
}

Device *device() { return zeal::_device; }

} // namespace zeal