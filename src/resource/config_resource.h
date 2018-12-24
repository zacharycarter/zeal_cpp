#ifndef ZEAL_CONFIG_RESOURCE_H_HEADER_GUARD
#define ZEAL_CONFIG_RESOURCE_H_HEADER_GUARD

#include "core/fs/types.h"
#include "core/memory/types.h"
#include "core/types.h"
#include "resource/types.h"

namespace zeal
{
namespace config_resource_internal
{
void compile(CompileOptions &opts);
void *load(File &file, Allocator &a);
void unload(Allocator &allocator, void *resource);

} // namespace config_resource_internal

} // namespace zeal

#endif // ZEAL_CONFIG_RESOURCE_H_HEADER_GUARD