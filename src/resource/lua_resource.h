#ifndef ZEAL_LUA_RESOURCE_H_HEADER_GUARD
#define ZEAL_LUA_RESOURCE_H_HEADER_GUARD

#include "core/fs/types.h"
#include "core/memory/types.h"
#include "resource/types.h"

namespace zeal
{
struct LuaResource
{
    u32 version;
    u32 size;
    //	char program[size]
};

namespace lua_resource_internal
{
void compile(CompileOptions &opts);

} // namespace lua_resource_internal

namespace lua_resource
{
/// Returns the lua program.
const char *program(const LuaResource *lr);

} // namespace lua_resource

} // namespace zeal

#endif // ZEAL_LUA_RESOURCE_H_HEADER_GUARD