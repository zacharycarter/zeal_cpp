#ifndef ZEAL_CONTAINER_TYPES_H_HEADER_GUARD
#define ZEAL_CONTAINER_TYPES_H_HEADER_GUARD

#include "core/functional.h"
#include "core/memory/types.h"
#include "core/pair.h"
#include "core/types.h"

/// @defgroup Containers Containers
/// @ingroup Core
namespace zeal
{
/// Dynamic array of POD items.
///
/// @note
/// Does not call constructors/destructors, uses
/// memcpy to move stuff around.
///
/// @ingroup Containers
template <typename T>
struct Array
{
	ALLOCATOR_AWARE;

	Allocator* _allocator;
	u32 _capacity;
	u32 _size;
	T* _data;

	Array(Allocator& a);
	Array(const Array<T>& other);
	~Array();
	T& operator[](u32 index);
	const T& operator[](u32 index) const;
	Array<T>& operator=(const Array<T>& other);
};

typedef Array<char> Buffer;

} // namespace zeal

#endif // ZEAL_CONTAINER_TYPES_H_HEADER_GUARD