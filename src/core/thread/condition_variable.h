#ifndef ZEAL_CONDITION_VARIABLE_H_HEADER_GUARD
#define ZEAL_CONDITION_VARIABLE_H_HEADER_GUARD

#include "core/thread/mutex.h"

namespace zeal
{
/// Condition variable.
///
/// @ingroup Thread
struct ConditionVariable
{
    ZE_ALIGN_DECL(16, u8 _data[64]);

    ///
    ConditionVariable();

    ///
    ~ConditionVariable();

    ///
    ConditionVariable(const ConditionVariable &) = delete;

    ///
    ConditionVariable &operator=(const ConditionVariable &) = delete;

    ///
    void wait(Mutex &mutex);

    ///
    void signal();
};

} // namespace zeal

#endif // ZEAL_CONDITION_VARIABLE_H_HEADER_GUARD