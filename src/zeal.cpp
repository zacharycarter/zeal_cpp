#include <zeal/c99/zeal.h>

namespace zeal
{
    bool init()
    {
        return true;
    }
} // namespace zeal

ZEAL_C_API bool zeal_init()
{
    return zeal::init();
}