#include "core/thread/mutex.h"
#include "core/error/error.h"
#include "core/platform.h"

#if ZEAL_PLATFORM_POSIX
#include <pthread.h>
#elif ZEAL_PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace zeal {
struct Private {
#if ZEAL_PLATFORM_POSIX
  pthread_mutex_t mutex;
#elif ZEAL_PLATFORM_WINDOWS
  CRITICAL_SECTION cs;
#endif
};

Mutex::Mutex() {
  ZE_STATIC_ASSERT(sizeof(_data) >= sizeof(Private));
  Private *priv = (Private *)_data;

#if ZEAL_PLATFORM_POSIX
  pthread_mutexattr_t attr;
  int err = pthread_mutexattr_init(&attr);
  ZE_ASSERT(err == 0, "pthread_mutexattr_init: errno = %d", err);
  err = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
  ZE_ASSERT(err == 0, "pthread_mutexattr_settype: errno = %d", err);
  err = pthread_mutex_init(&priv->mutex, &attr);
  ZE_ASSERT(err == 0, "pthread_mutex_init: errno = %d", err);
  err = pthread_mutexattr_destroy(&attr);
  ZE_ASSERT(err == 0, "pthread_mutexattr_destroy: errno = %d", err);
  ZE_UNUSED(err);
#elif ZEAL_PLATFORM_WINDOWS
  InitializeCriticalSection(&priv->cs);
#endif
}

Mutex::~Mutex() {
  Private *priv = (Private *)_data;

#if ZEAL_PLATFORM_POSIX
  int err = pthread_mutex_destroy(&priv->mutex);
  ZE_ASSERT(err == 0, "pthread_mutex_destroy: errno = %d", err);
  ZE_UNUSED(err);
#elif ZEAL_PLATFORM_WINDOWS
  DeleteCriticalSection(&priv->cs);
#endif
}

void Mutex::lock() {
  Private *priv = (Private *)_data;

#if ZEAL_PLATFORM_POSIX
  int err = pthread_mutex_lock(&priv->mutex);
  ZE_ASSERT(err == 0, "pthread_mutex_lock: errno = %d", err);
  ZE_UNUSED(err);
#elif ZEAL_PLATFORM_WINDOWS
  EnterCriticalSection(&priv->cs);
#endif
}

void Mutex::unlock() {
  Private *priv = (Private *)_data;

#if ZEAL_PLATFORM_POSIX
  int err = pthread_mutex_unlock(&priv->mutex);
  ZE_ASSERT(err == 0, "pthread_mutex_unlock: errno = %d", err);
  ZE_UNUSED(err);
#elif ZEAL_PLATFORM_WINDOWS
  LeaveCriticalSection(&priv->cs);
#endif
}

void *Mutex::native_handle() {
  Private *priv = (Private *)_data;

#if ZEAL_PLATFORM_POSIX
  return &priv->mutex;
#elif ZEAL_PLATFORM_WINDOWS
  return &priv->cs;
#endif
}

} // namespace zeal