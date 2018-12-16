#ifndef ZEAL_MUTEX_H_HEADER_GUARD
#define ZEAL_MUTEX_H_HEADER_GUARD

#include "core/types.h"

namespace zeal
{
/// Mutex.
///
/// @ingroup Thread
struct Mutex
{
	ZE_ALIGN_DECL(16, u8 _data[64]);

	///
	Mutex();

	///
	~Mutex();

	///
	Mutex(const Mutex&) = delete;

	///
	Mutex& operator=(const Mutex&) = delete;

	/// Locks the mutex.
	void lock();

	/// Unlocks the mutex.
	void unlock();

	/// Returns the native mutex handle.
	void* native_handle();
};

/// Automatically locks a mutex when created and unlocks when destroyed.
///
/// @ingroup Thread
struct ScopedMutex
{
	Mutex& _mutex;

	/// Locks the mutex @a m.
	ScopedMutex(Mutex& m)
		: _mutex(m)
	{
		_mutex.lock();
	}

	/// Unlocks the mutex passed to ScopedMutex::ScopedMutex()
	~ScopedMutex()
	{
		_mutex.unlock();
	}

	///
	ScopedMutex(const ScopedMutex&) = delete;

	///
	ScopedMutex& operator=(const ScopedMutex&) = delete;
};

} // namespace zeal

#endif // ZEAL_MUTEX_H_HEADER_GUARD