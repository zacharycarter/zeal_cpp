#include "core/platform.h"

#if ZEAL_PLATFORM_OSX

#include "core/containers/hash_map.h"
#include "core/fs/file_monitor.h"
#include "core/fs/path.h"
#include "core/memory/temp_allocator.h"
#include "core/strings/dynamic_string.h"
#include "core/thread/thread.h"
#include <CoreServices/CoreServices.h>
#include <iostream>

namespace zeal
{
const int eventModified = kFSEventStreamEventFlagItemFinderInfoMod |
                          kFSEventStreamEventFlagItemModified |
                          kFSEventStreamEventFlagItemInodeMetaMod |
                          kFSEventStreamEventFlagItemChangeOwner |
                          kFSEventStreamEventFlagItemXattrMod;

const int eventRenamed = kFSEventStreamEventFlagItemCreated |
                         kFSEventStreamEventFlagItemRemoved |
                         kFSEventStreamEventFlagItemRenamed;

const int eventSystem =
    kFSEventStreamEventFlagUserDropped | kFSEventStreamEventFlagKernelDropped |
    kFSEventStreamEventFlagEventIdsWrapped |
    kFSEventStreamEventFlagHistoryDone | kFSEventStreamEventFlagMount |
    kFSEventStreamEventFlagUnmount | kFSEventStreamEventFlagRootChanged;

struct FileMonitorImpl
{
    Allocator *_allocator;
    CFArrayRef _watches;
    FSEventStreamContext *_context;
    FSEventStreamRef _stream;
    Thread _thread;
    FileMonitorFunction _function;
    void *_user_data;

    FileMonitorImpl(Allocator &a)
        : _allocator(&a), _function(NULL), _user_data(NULL) {}

    static void watch(ConstFSEventStreamRef streamRef, void *clientCallBackInfo,
                      size_t numEvents, void *eventPaths,
                      const FSEventStreamEventFlags eventFlags[],
                      const FSEventStreamEventId eventIds[])
    {

        using namespace std;

        int i;
        char **paths = (char **)eventPaths;

        for (i = 0; i < numEvents; i++)
        {

            cout << "Changed " << endl
                 << paths[i];
            cout << "  Modified : " << !!(eventFlags[i] & eventModified) << endl;
            cout << "  Renamed  : " << !!(eventFlags[i] & eventRenamed) << endl;
            cout << "  System   : " << !!(eventFlags[i] & eventSystem) << endl;
        }
    }

    void add_watch(const char *path)
    {
        ZE_ENSURE(path != NULL);
        ZE_ASSERT(!path::has_trailing_separator(path), "Malformed path");

        CFStringRef dir =
            CFStringCreateWithCString(NULL, path, kCFStringEncodingUTF8);

        CFArrayRef _watches = CFArrayCreate(NULL, (const void **)&dir, 1, NULL);

        CFRelease(dir);

        CFAbsoluteTime latency = 3.0; /* Latency in seconds */

        _context = ZE_NEW(*_allocator, FSEventStreamContext);
        _context->version = 0;
        _context->info = _user_data;
        _context->retain = nullptr;
        _context->release = nullptr;
        _context->copyDescription = nullptr;

        _stream = FSEventStreamCreate(
            NULL, &FileMonitorImpl::watch,
            _context,                                // could put stream-specific data here.
                                                     // FSEventStreamRef stream;*/
            _watches, kFSEventStreamEventIdSinceNow, /* Or a previous event ID */
            latency,
            kFSEventStreamCreateFlagFileEvents /* Flags explained in reference:
https://developer.apple.com/library/mac/documentation/Darwin/Reference/FSEvents_Ref/Reference/reference.html
*/
        );
    }

    void start(const char *path, FileMonitorFunction fmf, void *user_data)
    {
        ZE_ENSURE(NULL != path);
        ZE_ENSURE(NULL != fmf);

        _function = fmf;
        _user_data = user_data;

        add_watch(path);

        _thread.start(run, this);
    }

    void stop()
    {
        FSEventStreamStop(_stream);
        FSEventStreamInvalidate(_stream);
        FSEventStreamRelease(_stream);
        CFRelease(_watches);
        _thread.stop();
        ZE_DELETE(*_allocator, _context);
    }

    static int run(void *thiz)
    {
        FileMonitorImpl *t = ((FileMonitorImpl *)thiz);
        FSEventStreamScheduleWithRunLoop(t->_stream, CFRunLoopGetCurrent(),
                                         kCFRunLoopDefaultMode);
        FSEventStreamStart(t->_stream);
        CFRunLoopRun();
        return 0;
    }
};

FileMonitor::FileMonitor(Allocator &a)
{
    _impl = ZE_NEW(a, FileMonitorImpl)(a);
}

FileMonitor::~FileMonitor() { ZE_DELETE(*_impl->_allocator, _impl); }

void FileMonitor::start(const char *path, bool recursive,
                        FileMonitorFunction fmf, void *user_data)
{
    _impl->start(path, fmf, user_data);
}

void FileMonitor::stop() { _impl->stop(); }

} // namespace zeal

#endif // ZEAL_PLATFORM_OSX