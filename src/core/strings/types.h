#ifndef ZEAL_STRING_TYPES_H_HEADER_GUARD
#define ZEAL_STRING_TYPES_H_HEADER_GUARD

#include "core/containers/types.h"

namespace zeal
{
struct DynamicString;
struct FixedString;
struct StringId32;
struct StringId64;

typedef StringId64 ResourceId;

/// Stream of characters.
///
/// @ingroup String
typedef Array<char> StringStream;

} // namespace zeal

#endif // ZEAL_STRING_TYPES_H_HEADER_GUARD