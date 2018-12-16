#ifndef ZEAL_PATH_TYPES_H_HEADER_GUARD
#define ZEAL_PATH_TYPES_H_HEADER_GUARD

namespace zeal
{
struct File;
struct FileMonitor;
struct Filesystem;

/// Enumerates file open modes.
///
/// @ingroup Filesystem
struct FileOpenMode
{
    enum Enum
    {
        READ,
        WRITE
    };
};

} // namespace zeal

#endif // ZEAL_PATH_TYPES_H_HEADER_GUARD