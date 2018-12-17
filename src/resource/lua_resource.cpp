#include "resource/lua_resource.h"
#include "config.h"
#include "core/containers/array.h"
#include "core/fs/file.h"
#include "core/memory/temp_allocator.h"
#include "core/strings/dynamic_string.h"
#include "core/strings/string_stream.h"
#include "resource/compile_options.h"

#define LUAJIT_NAME "./luajit"

#if ZEAL_PLATFORM_WINDOWS
#define EXE ".exe"
#else
#define EXE ""
#endif // ZEAL_PLATFORM_WINDOWS

#define LUAJIT_EXE LUAJIT_NAME EXE

#if ZEAL_DEBUG
#define LUAJIT_FLAGS "-bg" // Keep debug info
#else
#define LUAJIT_FLAGS "-b"
#endif // ZEAL_DEBUG

namespace zeal
{
namespace lua_resource_internal
{
void compile(CompileOptions &opts)
{
    TempAllocator1024 ta;
    DynamicString luasrc(ta);
    DynamicString luabin(ta);
    opts.get_absolute_path(opts.source_path(), luasrc);
    opts.get_temporary_path("lua", luabin);

    StringStream output(ta);
    const char *argv[] = {LUAJIT_EXE, LUAJIT_FLAGS, luasrc.c_str(),
                          luabin.c_str(), NULL};
    int ec = opts.run_external_compiler(argv, output);
    DATA_COMPILER_ASSERT(ec == 0, opts, "Failed to compile lua:\n%s",
                         string_stream::c_str(output));

    Buffer blob = opts.read_temporary(luabin.c_str());
    opts.delete_file(luabin.c_str());

    LuaResource lr;
    lr.version = RESOURCE_VERSION_SCRIPT;
    lr.size = array::size(blob);

    opts.write(lr.version);
    opts.write(lr.size);
    opts.write(blob);
}

} // namespace lua_resource_internal

namespace lua_resource
{
const char *program(const LuaResource *lr) { return (char *)&lr[1]; }

} // namespace lua_resource

} // namespace zeal