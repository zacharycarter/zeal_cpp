#include "lua/lua_stack.h"
#include "device/device.h"
#include "lua/lua_environment.h"

namespace zeal
{
bool LuaStack::is_vector3(int i)
{
    return device()->_lua_environment->is_vector3(
        (Vector3 *)lua_touserdata(L, i));
}

bool LuaStack::is_quaternion(int i)
{
    return device()->_lua_environment->is_quaternion(
        (Quaternion *)lua_touserdata(L, i));
}

bool LuaStack::is_matrix4x4(int i)
{
    return device()->_lua_environment->is_matrix4x4(
        (Matrix4x4 *)lua_touserdata(L, i));
}

#if ZEAL_DEBUG
void LuaStack::check_temporary(int i, const Vector3 *p)
{
    LuaEnvironment *env = device()->_lua_environment;
    if (!is_pointer(i) || !env->is_vector3(p))
    {
        luaL_typerror(L, i, "Vector3");
        ZE_UNREACHABLE();
    }
}

void LuaStack::check_temporary(int i, const Quaternion *p)
{
    LuaEnvironment *env = device()->_lua_environment;
    if (!is_pointer(i) || !env->is_quaternion(p))
    {
        luaL_typerror(L, i, "Quaternion");
        ZE_UNREACHABLE();
    }
}

void LuaStack::check_temporary(int i, const Matrix4x4 *p)
{
    LuaEnvironment *env = device()->_lua_environment;
    if (!is_pointer(i) || !env->is_matrix4x4(p))
    {
        luaL_typerror(L, i, "Matrix4x4");
        ZE_UNREACHABLE();
    }
}
#endif // ZEAL_DEBUG

void LuaStack::push_vector2(const Vector2 &v)
{
    Vector3 a;
    a.x = v.x;
    a.y = v.y;
    a.z = 0.0f;
    push_vector3(a);
}

void LuaStack::push_vector3(const Vector3 &v)
{
    lua_pushlightuserdata(L, device()->_lua_environment->next_vector3(v));
}

void LuaStack::push_quaternion(const Quaternion &q)
{
    lua_pushlightuserdata(L, device()->_lua_environment->next_quaternion(q));
}

void LuaStack::push_matrix4x4(const Matrix4x4 &m)
{
    lua_pushlightuserdata(L, device()->_lua_environment->next_matrix4x4(m));
}

void LuaStack::push_color4(const Color4 &c)
{
    // Color4 represented as Quaternion
    Quaternion q;
    q.x = c.x;
    q.y = c.y;
    q.z = c.z;
    q.w = c.w;
    push_quaternion(q);
}

} // namespace zeal