#ifndef ZEAL_SEMAPHORE_H_HEADER_GUARD
#define ZEAL_SEMAPHORE_H_HEADER_GUARD

#include "core/types.h"

namespace zeal
{
/// Semaphore.
///
/// @ingroup Thread.
struct Semaphore
{
    ZE_ALIGN_DECL(16, u8 _data[128]);

    ///
    Semaphore();

    ///
    ~Semaphore();

    ///
    Semaphore(const Semaphore &) = delete;

    ///
    Semaphore &operator=(const Semaphore &) = delete;

    ///
    void post(u32 count = 1);

    ///
    void wait();
};

} // namespace zeal

#endif // SEMAPHORE