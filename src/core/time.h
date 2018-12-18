#ifndef ZEAL_TIME_H_HEADER_GUARD
#define ZEAL_TIME_H_HEADER_GUARD

#include "core/types.h"

namespace zeal
{
namespace time
{
/// Returns the current time in ticks.
s64 now();

/// Returns the ticks in seconds.
f64 seconds(s64 ticks);

} // namespace time

} // namespace zeal

#endif // ZEAL_TIME_H_HEADER_GUARD