#include "resource/resource_loader.h"
#include "config.h"
#include "core/containers/hash_map.h"
#include "core/containers/queue.h"
#include "core/fs/file.h"
#include "core/fs/filesystem.h"
#include "core/fs/path.h"
#include "core/memory/memory.h"
#include "core/memory/temp_allocator.h"
#include "core/os.h"
#include "core/strings/dynamic_string.h"
#include "device/log.h"

LOG_SYSTEM(RESOURCE_LOADER, "resource_loader")

namespace zeal
{
static s32 thread_proc(void *thiz) { return ((ResourceLoader *)thiz)->run(); }

ResourceLoader::ResourceLoader(Filesystem &data_filesystem)
    : _data_filesystem(data_filesystem), _requests(default_allocator()),
      _loaded(default_allocator()), _fallback(default_allocator()),
      _exit(false)
{
    _thread.start(thread_proc, this);
}

ResourceLoader::~ResourceLoader()
{
    _exit = true;
    _requests_condition.signal(); // Spurious wake to exit thread
    _thread.stop();
}

void ResourceLoader::add_request(const ResourceRequest &rr)
{
    ScopedMutex sm(_mutex);
    queue::push_back(_requests, rr);
    _requests_condition.signal();
}

void ResourceLoader::flush()
{
    while (num_requests())
    {
    }
}

u32 ResourceLoader::num_requests()
{
    ScopedMutex sm(_mutex);
    return queue::size(_requests);
}

void ResourceLoader::add_loaded(ResourceRequest rr)
{
    ScopedMutex sm(_loaded_mutex);
    queue::push_back(_loaded, rr);
}

void ResourceLoader::get_loaded(Array<ResourceRequest> &loaded)
{
    ScopedMutex sm(_loaded_mutex);

    const u32 num = queue::size(_loaded);
    array::reserve(loaded, num);

    for (u32 i = 0; i < num; ++i)
    {
        array::push_back(loaded, queue::front(_loaded));
        queue::pop_front(_loaded);
    }
}

void ResourceLoader::register_fallback(StringId64 type, StringId64 name)
{
    hash_map::set(_fallback, type, name);
}

s32 ResourceLoader::run()
{
    while (1)
    {
        _mutex.lock();
        while (queue::empty(_requests) && !_exit)
            _requests_condition.wait(_mutex);

        if (_exit)
            break;

        ResourceRequest rr = queue::front(_requests);
        _mutex.unlock();

        StringId64 mix;
        mix._id = rr.type._id ^ rr.name._id;

        TempAllocator128 ta;
        DynamicString res_path(ta);
        mix.to_string(res_path);

        DynamicString path(ta);
        path::join(path, ZEAL_DATA_DIRECTORY, res_path.c_str());

        File *file = _data_filesystem.open(path.c_str(), FileOpenMode::READ);
        if (!file->is_open())
        {
            logw(RESOURCE_LOADER, "Can't load resource #ID(%s). Falling back...",
                 res_path.c_str());

            StringId64 fallback_name;
            fallback_name = hash_map::get(_fallback, rr.type, fallback_name);
            ZE_ENSURE(fallback_name._id != 0);

            mix._id = rr.type._id ^ fallback_name._id;
            mix.to_string(res_path);
            path::join(path, ZEAL_DATA_DIRECTORY, res_path.c_str());

            _data_filesystem.close(*file);
            file = _data_filesystem.open(path.c_str(), FileOpenMode::READ);
        }
        ZE_ASSERT(file->is_open(), "Can't load resource #ID(%s)", res_path.c_str());

        if (rr.load_function)
        {
            rr.data = rr.load_function(*file, *rr.allocator);
        }
        else
        {
            const u32 size = file->size();
            rr.data = rr.allocator->allocate(size);
            file->read(rr.data, size);
            ZE_ASSERT(*(u32 *)rr.data == rr.version, "Wrong version");
        }

        _data_filesystem.close(*file);

        add_loaded(rr);
        _mutex.lock();
        queue::pop_front(_requests);
        _mutex.unlock();
    }

    _mutex.unlock();
    return 0;
}

} // namespace zeal