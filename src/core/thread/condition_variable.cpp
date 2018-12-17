#include "core/thread/condition_variable.h"
#include "core/error/error.h"
#include "core/platform.h"

#if ZEAL_PLATFORM_POSIX
#include <pthread.h>
#elif ZEAL_PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace zeal
{
struct Private
{
#if ZEAL_PLATFORM_POSIX
    pthread_cond_t cond;
#elif ZEAL_PLATFORM_WINDOWS
    CONDITION_VARIABLE cv;
#endif
};

ConditionVariable::ConditionVariable()
{
    ZE_STATIC_ASSERT(sizeof(_data) >= sizeof(Private));
    Private *priv = (Private *)_data;

#if ZEAL_PLATFORM_POSIX
    int err = pthread_cond_init(&priv->cond, NULL);
    ZE_ASSERT(err == 0, "pthread_cond_init: errno = %d", err);
    ZE_UNUSED(err);
#elif ZEAL_PLATFORM_WINDOWS
    InitializeConditionVariable(&priv->cv);
#endif
}

ConditionVariable::~ConditionVariable()
{
    Private *priv = (Private *)_data;

#if ZEAL_PLATFORM_POSIX
    int err = pthread_cond_destroy(&priv->cond);
    ZE_ASSERT(err == 0, "pthread_cond_destroy: errno = %d", err);
    ZE_UNUSED(err);
#elif ZEAL_PLATFORM_WINDOWS
    // Do nothing
#endif
}

void ConditionVariable::wait(Mutex &mutex)
{
    Private *priv = (Private *)_data;

#if ZEAL_PLATFORM_POSIX
    int err =
        pthread_cond_wait(&priv->cond, (pthread_mutex_t *)mutex.native_handle());
    ZE_ASSERT(err == 0, "pthread_cond_wait: errno = %d", err);
    ZE_UNUSED(err);
#elif ZEAL_PLATFORM_WINDOWS
    SleepConditionVariableCS(&priv->cv, (CRITICAL_SECTION *)mutex.native_handle(),
                             INFINITE);
#endif
}

void ConditionVariable::signal()
{
    Private *priv = (Private *)_data;

#if ZEAL_PLATFORM_POSIX
    int err = pthread_cond_signal(&priv->cond);
    ZE_ASSERT(err == 0, "pthread_cond_signal: errno = %d", err);
    ZE_UNUSED(err);
#elif ZEAL_PLATFORM_WINDOWS
    WakeConditionVariable(&priv->cv);
#endif
}

} // namespace zeal