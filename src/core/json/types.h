#ifndef ZEAL_JSON_TYPES_H_HEADER_GUARD
#define ZEAL_JSON_TYPES_H_HEADER_GUARD

#include "core/containers/types.h"
#include "core/strings/fixed_string.h"

/// @defgroup JSON
/// @ingroup Core
namespace zeal
{
/// Enumerates JSON value types.
///
/// @ingroup JSON
struct JsonValueType
{
    enum Enum
    {
        NIL,
        BOOL,
        NUMBER,
        STRING,
        ARRAY,
        OBJECT
    };
};

/// Array of pointers to json-encoded data.
///
/// @ingroup JSON
typedef Array<const char *> JsonArray;

/// Map from key to pointers to json-encoded data.
///
/// @ingroup JSON
struct JsonObject
{
    Map<FixedString, const char *> _map;

    JsonObject(Allocator &a);

    const char *operator[](const char *key) const;
    const char *operator[](const FixedString &key) const;
};

} // namespace zeal

#endif // ZEAL_JSON_TYPES_H_HEADER_GUARD