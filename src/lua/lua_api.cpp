#include "core/guid.h"
#include "core/math/math.h"
#include "core/math/matrix4x4.h"
#include "core/math/types.h"
#include "core/math/vector2.h"
#include "core/math/vector3.h"
#include "core/memory/temp_allocator.h"
#include "core/strings/dynamic_string.h"
#include "core/strings/string_stream.h"
#include "device/console_server.h"
#include "device/device.h"
#include "device/profiler.h"
#include "lua/lua_environment.h"
#include "lua/lua_stack.h"
#include "resource/resource_manager.h"

namespace zeal
{
static int vector3_ctor(lua_State *L)
{
    LuaStack stack(L);
    Vector3 v;
    v.x = stack.get_float(1 + 1);
    v.y = stack.get_float(2 + 1);
    v.z = stack.get_float(3 + 1);
    stack.push_vector3(v);
    return 1;
}

static int vector3_x(lua_State *L)
{
    LuaStack stack(L);
    stack.push_float(stack.get_vector3(1).x);
    return 1;
}

static int vector3_y(lua_State *L)
{
    LuaStack stack(L);
    stack.push_float(stack.get_vector3(1).y);
    return 1;
}

static int vector3_z(lua_State *L)
{
    LuaStack stack(L);
    stack.push_float(stack.get_vector3(1).z);
    return 1;
}

static int vector3_set_x(lua_State *L)
{
    LuaStack stack(L);
    stack.get_vector3(1).x = stack.get_float(2);
    return 0;
}

static int vector3_set_y(lua_State *L)
{
    LuaStack stack(L);
    stack.get_vector3(1).y = stack.get_float(2);
    return 0;
}

static int vector3_set_z(lua_State *L)
{
    LuaStack stack(L);
    stack.get_vector3(1).z = stack.get_float(2);
    return 0;
}

static int vector3_elements(lua_State *L)
{
    LuaStack stack(L);
    Vector3 &a = stack.get_vector3(1);
    stack.push_float(a.x);
    stack.push_float(a.y);
    stack.push_float(a.z);
    return 3;
}

static int vector3_add(lua_State *L)
{
    LuaStack stack(L);
    stack.push_vector3(stack.get_vector3(1) + stack.get_vector3(2));
    return 1;
}

static int vector3_subtract(lua_State *L)
{
    LuaStack stack(L);
    stack.push_vector3(stack.get_vector3(1) - stack.get_vector3(2));
    return 1;
}

static int vector3_multiply(lua_State *L)
{
    LuaStack stack(L);
    stack.push_vector3(stack.get_vector3(1) * stack.get_float(2));
    return 1;
}

static int vector3_dot(lua_State *L)
{
    LuaStack stack(L);
    stack.push_float(dot(stack.get_vector3(1), stack.get_vector3(2)));
    return 1;
}

static int vector3_cross(lua_State *L)
{
    LuaStack stack(L);
    stack.push_vector3(cross(stack.get_vector3(1), stack.get_vector3(2)));
    return 1;
}

static int vector3_equal(lua_State *L)
{
    LuaStack stack(L);
    stack.push_bool(stack.get_vector3(1) == stack.get_vector3(2));
    return 1;
}

static int vector3_length(lua_State *L)
{
    LuaStack stack(L);
    stack.push_float(length(stack.get_vector3(1)));
    return 1;
}

static int vector3_length_squared(lua_State *L)
{
    LuaStack stack(L);
    stack.push_float(length_squared(stack.get_vector3(1)));
    return 1;
}

static int vector3_set_length(lua_State *L)
{
    LuaStack stack(L);
    set_length(stack.get_vector3(1), stack.get_float(2));
    return 0;
}

static int vector3_normalize(lua_State *L)
{
    LuaStack stack(L);
    stack.push_vector3(normalize(stack.get_vector3(1)));
    return 1;
}

static int vector3_distance(lua_State *L)
{
    LuaStack stack(L);
    stack.push_float(distance(stack.get_vector3(1), stack.get_vector3(2)));
    return 1;
}

static int vector3_distance_squared(lua_State *L)
{
    LuaStack stack(L);
    stack.push_float(distance_squared(stack.get_vector3(1), stack.get_vector3(2)));
    return 1;
}

static int vector3_angle(lua_State *L)
{
    LuaStack stack(L);
    stack.push_float(angle(stack.get_vector3(1), stack.get_vector3(2)));
    return 1;
}

static int vector3_max(lua_State *L)
{
    LuaStack stack(L);
    stack.push_vector3(max(stack.get_vector3(1), stack.get_vector3(2)));
    return 1;
}

static int vector3_min(lua_State *L)
{
    LuaStack stack(L);
    stack.push_vector3(min(stack.get_vector3(1), stack.get_vector3(2)));
    return 1;
}

static int vector3_lerp(lua_State *L)
{
    LuaStack stack(L);
    stack.push_vector3(lerp(stack.get_vector3(1), stack.get_vector3(2), stack.get_float(3)));
    return 1;
}

static int vector3_forward(lua_State *L)
{
    LuaStack stack(L);
    stack.push_vector3(VECTOR3_FORWARD);
    return 1;
}

static int vector3_backward(lua_State *L)
{
    LuaStack stack(L);
    stack.push_vector3(VECTOR3_BACKWARD);
    return 1;
}

static int vector3_left(lua_State *L)
{
    LuaStack stack(L);
    stack.push_vector3(VECTOR3_LEFT);
    return 1;
}

static int vector3_right(lua_State *L)
{
    LuaStack stack(L);
    stack.push_vector3(VECTOR3_RIGHT);
    return 1;
}

static int vector3_up(lua_State *L)
{
    LuaStack stack(L);
    stack.push_vector3(VECTOR3_UP);
    return 1;
}

static int vector3_down(lua_State *L)
{
    LuaStack stack(L);
    stack.push_vector3(VECTOR3_DOWN);
    return 1;
}

static int vector3_zero(lua_State *L)
{
    LuaStack stack(L);
    stack.push_vector3(VECTOR3_ZERO);
    return 1;
}

static int vector3_to_string(lua_State *L)
{
    LuaStack stack(L);
    const Vector3 v = stack.get_vector3(1);
    char buf[256];
    snprintf(buf, sizeof(buf), "%.4f %.4f %.4f", v.x, v.y, v.z);
    stack.push_string(buf);
    return 1;
}

static int vector2_ctor(lua_State *L)
{
    LuaStack stack(L);
    Vector2 v;
    v.x = stack.get_float(1 + 1);
    v.y = stack.get_float(2 + 1);
    stack.push_vector2(v);
    return 1;
}

static int vector3box_ctor(lua_State *L)
{
    LuaStack stack(L);

    if (stack.num_args() == 0 + 1)
    {
        stack.push_vector3box(VECTOR3_ZERO);
    }
    else if (stack.num_args() == 1 + 1)
    {
        stack.push_vector3box(stack.get_vector3(1 + 1));
    }
    else
    {
        Vector3 v;
        v.x = stack.get_float(1 + 1);
        v.y = stack.get_float(2 + 1);
        v.z = stack.get_float(3 + 1);
        stack.push_vector3box(v);
    }

    return 1;
}

static int vector3box_store(lua_State *L)
{
    LuaStack stack(L);

    Vector3 &v = stack.get_vector3box(1);

    if (stack.num_args() == 2)
        v = stack.get_vector3(2);
    else
        v = vector3(stack.get_float(2), stack.get_float(3), stack.get_float(4));

    return 0;
}

static int vector3box_unbox(lua_State *L)
{
    LuaStack stack(L);
    stack.push_vector3(stack.get_vector3box(1));
    return 1;
}

static int vector3box_tostring(lua_State *L)
{
    LuaStack stack(L);
    Vector3 &v = stack.get_vector3box(1);
    stack.push_fstring("Vector3Box (%p)", &v);
    return 1;
}

void load_api(LuaEnvironment &env)
{
    env.add_module_function("Vector3", "x", vector3_x);
    env.add_module_function("Vector3", "y", vector3_y);
    env.add_module_function("Vector3", "z", vector3_z);
    env.add_module_function("Vector3", "set_x", vector3_set_x);
    env.add_module_function("Vector3", "set_y", vector3_set_y);
    env.add_module_function("Vector3", "set_z", vector3_set_z);
    env.add_module_function("Vector3", "elements", vector3_elements);
    env.add_module_function("Vector3", "add", vector3_add);
    env.add_module_function("Vector3", "subtract", vector3_subtract);
    env.add_module_function("Vector3", "multiply", vector3_multiply);
    env.add_module_function("Vector3", "dot", vector3_dot);
    env.add_module_function("Vector3", "cross", vector3_cross);
    env.add_module_function("Vector3", "equal", vector3_equal);
    env.add_module_function("Vector3", "length", vector3_length);
    env.add_module_function("Vector3", "length_squared", vector3_length_squared);
    env.add_module_function("Vector3", "set_length", vector3_set_length);
    env.add_module_function("Vector3", "normalize", vector3_normalize);
    env.add_module_function("Vector3", "distance", vector3_distance);
    env.add_module_function("Vector3", "distance_squared", vector3_distance_squared);
    env.add_module_function("Vector3", "angle", vector3_angle);
    env.add_module_function("Vector3", "max", vector3_max);
    env.add_module_function("Vector3", "min", vector3_min);
    env.add_module_function("Vector3", "lerp", vector3_lerp);
    env.add_module_function("Vector3", "forward", vector3_forward);
    env.add_module_function("Vector3", "backward", vector3_backward);
    env.add_module_function("Vector3", "left", vector3_left);
    env.add_module_function("Vector3", "right", vector3_right);
    env.add_module_function("Vector3", "up", vector3_up);
    env.add_module_function("Vector3", "down", vector3_down);
    env.add_module_function("Vector3", "zero", vector3_zero);
    env.add_module_function("Vector3", "to_string", vector3_to_string);
    env.add_module_metafunction("Vector3", "__call", vector3_ctor);
}

} // namespace zeal