#ifndef ZEAL_FILE_MONITOR_H_HEADER_GUARD
#define ZEAL_FILE_MONITOR_H_HEADER_GUARD

#include "core/memory/types.h"
#include "core/types.h"

namespace zeal
{
/// Enumerates file monitor events.
///
/// @ingroup Filesystem
struct FileMonitorEvent
{
    enum Enum
    {
        CREATED,
        DELETED,
        RENAMED,
        CHANGED,

        COUNT
    };
};

typedef void (*FileMonitorFunction)(void *user_data, FileMonitorEvent::Enum fme, bool is_dir, const char *path, const char *path_modified);

struct FileMonitorImpl;

/// Watches a directory for changes.
///
/// @ingroup Filesystem
struct FileMonitor
{
    FileMonitorImpl *_impl;

    FileMonitor(Allocator &a);
    ~FileMonitor();

    // Starts monitoring the @a path.
    void start(const char *path, bool recursive, FileMonitorFunction fmf, void *user_data);

    // Stops monitoring.
    void stop();
};

} // namespace zeal

#endif // ZEAL_FILE_MONITOR_H_HEADER_GUARD