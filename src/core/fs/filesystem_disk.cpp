#include "core/fs/filesystem_disk.h"
#include "core/containers/vector.h"
#include "core/fs/file.h"
#include "core/fs/path.h"
#include "core/memory/temp_allocator.h"
#include "core/os.h"
#include "core/strings/dynamic_string.h"

#if ZEAL_PLATFORM_POSIX
#include <errno.h>
#include <stdio.h>
#elif ZEAL_PLATFORM_WINDOWS
#include <tchar.h>
#include <windows.h>
#endif

namespace zeal
{
struct FileDisk : public File
{
#if ZEAL_PLATFORM_POSIX
    FILE *_file;
#elif ZEAL_PLATFORM_WINDOWS
    HANDLE _file;
    bool _eof;
#endif

    /// Opens the file located at @a path with the given @a mode.
    FileDisk()
#if ZEAL_PLATFORM_POSIX
        : _file(NULL)
#elif ZEAL_PLATFORM_WINDOWS
        : _file(INVALID_HANDLE_VALUE), _eof(false)
#endif
    {
    }

    virtual ~FileDisk() { close(); }

    void open(const char *path, FileOpenMode::Enum mode)
    {
#if ZEAL_PLATFORM_POSIX
        _file = fopen(path, (mode == FileOpenMode::READ) ? "rb" : "wb");
#elif ZEAL_PLATFORM_WINDOWS
        _file = CreateFile(
            path, (mode == FileOpenMode::READ) ? GENERIC_READ : GENERIC_WRITE, 0,
            NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
    }

    void close()
    {
        if (is_open())
        {
#if ZEAL_PLATFORM_POSIX
            fclose(_file);
            _file = NULL;
#elif ZEAL_PLATFORM_WINDOWS
            CloseHandle(_file);
            _file = INVALID_HANDLE_VALUE;
#endif
        }
    }

    bool is_open()
    {
#if ZEAL_PLATFORM_POSIX
        return _file != NULL;
#elif ZEAL_PLATFORM_WINDOWS
        return _file != INVALID_HANDLE_VALUE;
#endif
    }

    u32 size()
    {
        ZE_ASSERT(is_open(), "File is not open");
#if ZEAL_PLATFORM_POSIX
        long pos = ftell(_file);
        ZE_ASSERT(pos != -1, "ftell: errno = %d", errno);
        int err = fseek(_file, 0, SEEK_END);
        ZE_ASSERT(err == 0, "fseek: errno = %d", errno);
        long size = ftell(_file);
        ZE_ASSERT(size != -1, "ftell: errno = %d", errno);
        err = fseek(_file, (long)pos, SEEK_SET);
        ZE_ASSERT(err == 0, "fseek: errno = %d", errno);
        ZE_UNUSED(err);
        return (u32)size;
#elif ZEAL_PLATFORM_WINDOWS
        return GetFileSize(_file, NULL);
#endif
    }

    u32 position()
    {
        ZE_ASSERT(is_open(), "File is not open");
#if ZEAL_PLATFORM_POSIX
        long pos = ftell(_file);
        ZE_ASSERT(pos != -1, "ftell: errno = %d", errno);
        return (u32)pos;
#elif ZEAL_PLATFORM_WINDOWS
        DWORD pos = SetFilePointer(_file, 0, NULL, FILE_CURRENT);
        ZE_ASSERT(pos != INVALID_SET_FILE_POINTER,
                  "SetFilePointer: GetLastError = %d", GetLastError());
        return (u32)pos;
#endif
    }

    bool end_of_file()
    {
        ZE_ASSERT(is_open(), "File is not open");
#if ZEAL_PLATFORM_POSIX
        return feof(_file) != 0;
#elif ZEAL_PLATFORM_WINDOWS
        return _eof;
#endif
    }

    void seek(u32 position)
    {
        ZE_ASSERT(is_open(), "File is not open");
#if ZEAL_PLATFORM_POSIX
        int err = fseek(_file, (long)position, SEEK_SET);
        ZE_ASSERT(err == 0, "fseek: errno = %d", errno);
#elif ZEAL_PLATFORM_WINDOWS
        DWORD err = SetFilePointer(_file, position, NULL, FILE_BEGIN);
        ZE_ASSERT(err != INVALID_SET_FILE_POINTER,
                  "SetFilePointer: GetLastError = %d", GetLastError());
#endif
        ZE_UNUSED(err);
    }

    void seek_to_end()
    {
        ZE_ASSERT(is_open(), "File is not open");
#if ZEAL_PLATFORM_POSIX
        int err = fseek(_file, 0, SEEK_END);
        ZE_ASSERT(err == 0, "fseek: errno = %d", errno);
#elif ZEAL_PLATFORM_WINDOWS
        DWORD err = SetFilePointer(_file, 0, NULL, FILE_END);
        ZE_ASSERT(err != INVALID_SET_FILE_POINTER,
                  "SetFilePointer: GetLastError = %d", GetLastError());
#endif
        ZE_UNUSED(err);
    }

    void skip(u32 bytes)
    {
        ZE_ASSERT(is_open(), "File is not open");
#if ZEAL_PLATFORM_POSIX
        int err = fseek(_file, bytes, SEEK_CUR);
        ZE_ASSERT(err == 0, "fseek: errno = %d", errno);
#elif ZEAL_PLATFORM_WINDOWS
        DWORD err = SetFilePointer(_file, bytes, NULL, FILE_CURRENT);
        ZE_ASSERT(err != INVALID_SET_FILE_POINTER,
                  "SetFilePointer: GetLastError = %d", GetLastError());
#endif
        ZE_UNUSED(err);
    }

    u32 read(void *data, u32 size)
    {
        ZE_ASSERT(is_open(), "File is not open");
        ZE_ASSERT(data != NULL, "Data must be != NULL");
#if ZEAL_PLATFORM_POSIX
        size_t bytes_read = fread(data, 1, size, _file);
        ZE_ASSERT(ferror(_file) == 0, "fread error");
        return (u32)bytes_read;
#elif ZEAL_PLATFORM_WINDOWS
        DWORD bytes_read;
        BOOL err = ReadFile(_file, data, size, &bytes_read, NULL);
        ZE_ASSERT(err == TRUE, "ReadFile: GetLastError = %d", GetLastError());
        _eof = err && bytes_read == 0;
        return bytes_read;
#endif
    }

    u32 write(const void *data, u32 size)
    {
        ZE_ASSERT(is_open(), "File is not open");
        ZE_ASSERT(data != NULL, "Data must be != NULL");
#if ZEAL_PLATFORM_POSIX
        size_t bytes_written = fwrite(data, 1, size, _file);
        ZE_ASSERT(ferror(_file) == 0, "fwrite error");
        return (u32)bytes_written;
#elif ZEAL_PLATFORM_WINDOWS
        DWORD bytes_written;
        WriteFile(_file, data, size, &bytes_written, NULL);
        ZE_ASSERT(size == bytes_written, "WriteFile: GetLastError = %d",
                  GetLastError());
        return bytes_written;
#endif
    }

    void flush()
    {
        ZE_ASSERT(is_open(), "File is not open");
#if ZEAL_PLATFORM_POSIX
        int err = fflush(_file);
        ZE_ASSERT(err == 0, "fflush: errno = %d", errno);
#elif ZEAL_PLATFORM_WINDOWS
        BOOL err = FlushFileBuffers(_file);
        ZE_ASSERT(err != 0, "FlushFileBuffers: GetLastError = %d", GetLastError());
#endif
        ZE_UNUSED(err);
    }
};

FilesystemDisk::FilesystemDisk(Allocator &a) : _allocator(&a), _prefix(a) {}

void FilesystemDisk::set_prefix(const char *prefix)
{
    _prefix.set(prefix, strlen32(prefix));
}

File *FilesystemDisk::open(const char *path, FileOpenMode::Enum mode)
{
    ZE_ENSURE(NULL != path);

    TempAllocator256 ta;
    DynamicString abs_path(ta);
    get_absolute_path(path, abs_path);

    FileDisk *file = ZE_NEW(*_allocator, FileDisk)();
    file->open(abs_path.c_str(), mode);
    return file;
}

void FilesystemDisk::close(File &file) { ZE_DELETE(*_allocator, &file); }

bool FilesystemDisk::exists(const char *path)
{
    ZE_ENSURE(NULL != path);

    TempAllocator256 ta;
    DynamicString abs_path(ta);
    get_absolute_path(path, abs_path);

    Stat info;
    os::stat(info, abs_path.c_str());
    return info.file_type != Stat::NO_ENTRY;
}

bool FilesystemDisk::is_directory(const char *path)
{
    ZE_ENSURE(NULL != path);

    TempAllocator256 ta;
    DynamicString abs_path(ta);
    get_absolute_path(path, abs_path);

    Stat info;
    os::stat(info, abs_path.c_str());
    return info.file_type == Stat::DIRECTORY;
}

bool FilesystemDisk::is_file(const char *path)
{
    ZE_ENSURE(NULL != path);

    TempAllocator256 ta;
    DynamicString abs_path(ta);
    get_absolute_path(path, abs_path);

    Stat info;
    os::stat(info, abs_path.c_str());
    return info.file_type == Stat::REGULAR;
}

u64 FilesystemDisk::last_modified_time(const char *path)
{
    ZE_ENSURE(NULL != path);

    TempAllocator256 ta;
    DynamicString abs_path(ta);
    get_absolute_path(path, abs_path);

    Stat info;
    os::stat(info, abs_path.c_str());
    return info.mtime;
}

void FilesystemDisk::create_directory(const char *path)
{
    ZE_ENSURE(NULL != path);

    TempAllocator256 ta;
    DynamicString abs_path(ta);
    get_absolute_path(path, abs_path);

    Stat info;
    os::stat(info, abs_path.c_str());
    if (info.file_type != Stat::NO_ENTRY)
        return;

    os::create_directory(abs_path.c_str());
}

void FilesystemDisk::delete_directory(const char *path)
{
    ZE_ENSURE(NULL != path);

    TempAllocator256 ta;
    DynamicString abs_path(ta);
    get_absolute_path(path, abs_path);

    os::delete_directory(abs_path.c_str());
}

void FilesystemDisk::delete_file(const char *path)
{
    ZE_ENSURE(NULL != path);

    TempAllocator256 ta;
    DynamicString abs_path(ta);
    get_absolute_path(path, abs_path);

    os::delete_file(abs_path.c_str());
}

void FilesystemDisk::list_files(const char *path,
                                Vector<DynamicString> &files)
{
    ZE_ENSURE(NULL != path);

    TempAllocator256 ta;
    DynamicString abs_path(ta);
    get_absolute_path(path, abs_path);

    os::list_files(abs_path.c_str(), files);
}

void FilesystemDisk::get_absolute_path(const char *path,
                                       DynamicString &os_path)
{
    if (path::is_absolute(path))
    {
        os_path = path;
        return;
    }

    TempAllocator1024 ta;
    DynamicString str(ta);
    path::join(str, _prefix.c_str(), path);
    path::reduce(os_path, str.c_str());
}

} // namespace zeal