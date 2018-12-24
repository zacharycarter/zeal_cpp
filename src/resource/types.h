#ifndef ZEAL_RESOURCE_TYPES_H_HEADER_GUARD
#define ZEAL_RESOURCE_TYPES_H_HEADER_GUARD

/// @defgroup Resource Resource
namespace zeal
{
struct CompileOptions;
struct DataCompiler;
struct ResourceLoader;
struct ResourceManager;
struct ResourcePackage;

struct LuaResource;

} // namespace zeal

/// @addtogroup Resource
/// @{
#define RESOURCE_TYPE_CONFIG StringId64(0x82645835e6b73232)
#define RESOURCE_TYPE_MATERIAL StringId64(0xeac0b497876adedf)
#define RESOURCE_TYPE_SCRIPT StringId64(0xa14e8dfa2cd117e2)
#define RESOURCE_TYPE_TEXTURE StringId64(0xcd4238c6a0c69e32)
#define RESOURCE_TYPE_UNIT StringId64(0xe0a48d0be9a7453f)

#define RESOURCE_VERSION_CONFIG u32(1)
#define RESOURCE_VERSION_MATERIAL u32(1)
#define RESOURCE_VERSION_SCRIPT u32(1)
#define RESOURCE_VERSION_TEXTURE u32(1)
#define RESOURCE_VERSION_UNIT u32(1)
/// @}

#endif // ZEAL_RESOURCE_TYPES_H_HEADER_GUARD