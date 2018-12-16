#ifndef ZEAL_ALLOCATOR_H_HEADER_GUARD
#define ZEAL_ALLOCATOR_H_HEADER_GUARD

#include "core/types.h"

namespace zeal
{
/// Interface for memory allocators.
///
/// @ingroup Memory
struct Allocator
{
    Allocator() {}
    virtual	~Allocator() {}
    Allocator(const Allocator&) = delete;
    Allocator& operator=(const Allocator&) = delete;

    /// Allocates @a size bytes of memory aligned to the specified
	/// @a align byte and returns a pointer to the first allocated byte.
    virtual void* allocate(u32 size, u32 align = DEFAULT_ALIGN) = 0;

    /// Deallocates a previously allocated block of memory pointed by @a data.
    virtual void deallocate(void* data) = 0;

    /// Returns the size of the memory block pointed by @a ptr or SIZE_NOT_TRACKED
	/// if the allocator does not support memory tracking.
	/// @a ptr must be a pointer returned by Allocator::allocate().
    virtual u32 allocated_size(const void* ptr) = 0;

    /// Returns the total number of bytes allocated.
    virtual u32 total_allocated() = 0;

    /// Default memory alignment in bytes.
	static const u32 DEFAULT_ALIGN = 4;
    static const u32 SIZE_NOT_TRACKED = 0xffffffffu;
};

} // namespace zeal

#endif // ZEAL_ALLOCATOR_H_HEADER_GUARD