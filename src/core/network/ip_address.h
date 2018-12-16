#ifndef ZEAL_IP_ADDRESS_H_HEADER_GUARD
#define ZEAL_IP_ADDRESS_H_HEADER_GUARD

#include "core/types.h"

namespace zeal
{
/// IP address.
///
/// @ingroup Network
struct IPAddress
{
    u8 a, b, c, d;

    /// Returns the IP address as packed 32-bit integer.
    u32 address() const
    {
        return (u32(a) << 24) | (u32(b) << 16) | (u32(c) << 8) | u32(d);
    }
};

const IPAddress IP_ADDRESS_LOOPBACK = {127, 0, 0, 1};

} // namespace zeal

#endif // ZEAL_IP_ADDRESS_H_HEADER_GUARD