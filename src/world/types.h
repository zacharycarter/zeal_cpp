#ifndef ZEAL_WORLD_TYPES_H_HEADER_GUARD
#define ZEAL_WORLD_TYPES_H_HEADER_GUARD

#include "core/functional.h"
#include "core/math/types.h"
#include "core/strings/string_id.h"
#include "core/types.h"

namespace zeal
{
struct AnimationStateMachine;
struct DebugLine;
struct Gui;
struct Level;
struct Material;
struct PhysicsWorld;
struct RenderWorld;
struct SceneGraph;
struct ScriptWorld;
struct SoundWorld;
struct World;

typedef u32 SoundInstanceId;

#define UNIT_INDEX_BITS 22
#define UNIT_INDEX_MASK 0x003fffff
#define UNIT_ID_BITS 8
#define UNIT_ID_MASK 0x3fc00000

/// Unit id.
///
/// @ingroup World
struct UnitId
{
    u32 _idx;

    u32 index() const { return _idx & UNIT_INDEX_MASK; }

    u32 id() const { return (_idx >> UNIT_INDEX_BITS) & UNIT_ID_MASK; }

    bool is_valid() { return _idx != UINT32_MAX; }
};

inline bool operator==(const UnitId &a, const UnitId &b)
{
    return a._idx == b._idx;
}

const UnitId UNIT_INVALID = {UINT32_MAX};

template <>
struct hash<UnitId>
{
    u32 operator()(const UnitId &id) const { return id._idx; }
};

#define INSTANCE_ID(name) \
    struct name           \
    {                     \
        u32 i;            \
    };                    \
    inline bool is_valid(name inst) { return inst.i != UINT32_MAX; }

INSTANCE_ID(TransformInstance);
INSTANCE_ID(CameraInstance);
INSTANCE_ID(MeshInstance);
INSTANCE_ID(SpriteInstance);
INSTANCE_ID(LightInstance);
INSTANCE_ID(ColliderInstance);
INSTANCE_ID(ActorInstance);
INSTANCE_ID(JointInstance);
INSTANCE_ID(ScriptInstance);

#undef INSTANCE_ID

} // namespace zeal

#endif // ZEAL_WORLD_TYPES_H_HEADER_GUARD