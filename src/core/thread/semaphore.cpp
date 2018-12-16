#include "core/thread/semaphore.h"
#include "core/error/error.h"
#include "core/platform.h"

#if ZEAL_PLATFORM_POSIX
#include <pthread.h>
#elif ZEAL_PLATFORM_WINDOWS
#include <limits.h> // LONG_MAX
#include <windows.h>
#endif

namespace zeal
{
struct Private
{
#if ZEAL_PLATFORM_POSIX
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    s32 count;
#elif ZEAL_PLATFORM_WINDOWS
    HANDLE handle;
#endif
};

Semaphore::Semaphore()
{
    ZE_STATIC_ASSERT(sizeof(_data) >= sizeof(Private));
    Private *priv = (Private *)_data;

#if ZEAL_PLATFORM_POSIX
    int err = 0;
    err = pthread_mutex_init(&priv->mutex, NULL);
    ZE_ASSERT(err == 0, "pthread_mutex_init: errno = %d", err);
    err = pthread_cond_init(&priv->cond, NULL);
    ZE_ASSERT(err == 0, "pthread_cond_init: errno = %d", err);
    ZE_UNUSED(err);
    priv->count = 0;
#elif ZEAL_PLATFORM_WINDOWS
    priv->handle = CreateSemaphore(NULL, 0, LONG_MAX, NULL);
    ZE_ASSERT(priv->handle != NULL, "CreateSemaphore: GetLastError = %d",
              GetLastError());
    ZE_UNUSED(priv->handle);
#endif
}

Semaphore::~Semaphore()
{
    Private *priv = (Private *)_data;

#if ZEAL_PLATFORM_POSIX
    int err = pthread_cond_destroy(&priv->cond);
    ZE_ASSERT(err == 0, "pthread_cond_destroy: errno = %d", err);
    ZE_UNUSED(err);
#elif ZEAL_PLATFORM_WINDOWS
    BOOL err = CloseHandle(priv->handle);
    ZE_ASSERT(err != 0, "CloseHandle: GetLastError = %d", GetLastError());
    ZE_UNUSED(err);
#endif
}

void Semaphore::post(u32 count)
{
    Private *priv = (Private *)_data;

#if ZEAL_PLATFORM_POSIX
    pthread_mutex_lock(&priv->mutex);
    for (u32 i = 0; i < count; ++i)
    {
        int err = pthread_cond_signal(&priv->cond);
        ZE_ASSERT(err == 0, "pthread_cond_signal: errno = %d", err);
        ZE_UNUSED(err);
    }

    priv->count += count;
    pthread_mutex_unlock(&priv->mutex);
#elif ZEAL_PLATFORM_WINDOWS
    BOOL err = ReleaseSemaphore(priv->handle, count, NULL);
    ZE_ASSERT(err != 0, "ReleaseSemaphore: GetLastError = %d", GetLastError());
    ZE_UNUSED(err);
#endif
}

void Semaphore::wait()
{
    Private *priv = (Private *)_data;

#if ZEAL_PLATFORM_POSIX
    pthread_mutex_lock(&priv->mutex);
    while (priv->count <= 0)
    {
        int err = pthread_cond_wait(&priv->cond, &priv->mutex);
        ZE_ASSERT(err == 0, "pthread_cond_wait: errno = %d", err);
        ZE_UNUSED(err);
    }
    priv->count--;
    pthread_mutex_unlock(&priv->mutex);
#elif ZEAL_PLATFORM_WINDOWS
    DWORD err = WaitForSingleObject(priv->handle, INFINITE);
    ZE_ASSERT(err == WAIT_OBJECT_0, "WaitForSingleObject: GetLastError = %d",
              GetLastError());
    ZE_UNUSED(err);
#endif
}

} // namespace zeal