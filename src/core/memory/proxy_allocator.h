#ifndef ZEAL_PROXY_ALLOCATOR_H_HEADER_GUARD
#define ZEAL_PROXY_ALLOCATOR_H_HEADER_GUARD

#include "core/memory/allocator.h"

namespace zeal
{
/// Offers the facility to tag allocators by a string identifier.
/// Proxy allocator is appended to a global linked list when instantiated
/// so that it is possible to later visit that list for debugging purposes.
///
/// @ingroup Memory
struct ProxyAllocator : public Allocator
{
    Allocator &_allocator;
    const char *_name;

    /// Tag all allocations made with @a allocator by the given @a name
    ProxyAllocator(Allocator &allocator, const char *name);

    /// @copydoc Allocator::allocate()
    void *allocate(u32 size, u32 align = Allocator::DEFAULT_ALIGN);

    /// @copydoc Allocator::deallocate()
    void deallocate(void *data);

    /// @copydoc Allocator::allocated_size()
    u32 allocated_size(const void * /*ptr*/) { return SIZE_NOT_TRACKED; }

    /// @copydoc Allocator::total_allocated()
    u32 total_allocated() { return SIZE_NOT_TRACKED; }

    /// Returns the name of the proxy allocator
    const char *name() const;
};

} // namespace zeal

#endif // ZEAL_PROXY_ALLOCATOR_H_HEADER_GUARD