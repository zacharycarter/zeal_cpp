#ifndef ZEAL_MURMUR_H_HEADER_GUARD
#define ZEAL_MURMUR_H_HEADER_GUARD

#include "core/types.h"

namespace zeal
{
u32 murmur32(const void* key, u32 len, u32 seed);
u64 murmur64(const void* key, u32 len, u64 seed);

} // namespace zeal

#endif // ZEAL_MURMUR_H_HEADER_GUARD