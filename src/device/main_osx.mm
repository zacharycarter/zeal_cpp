#include "config.h"

#if ZEAL_PLATFORM_OSX

#include "device/device.h"
#import <Cocoa/Cocoa.h>

namespace zeal
{

} // namespace zeal

struct InitMemoryGlobals
{
	InitMemoryGlobals()
	{
		zeal::memory_globals::init();
	}

	~InitMemoryGlobals()
	{
		zeal::memory_globals::shutdown();
	}
};

int main(int _argc, const char* const* _argv)
{
    using namespace zeal;
    
    InitMemoryGlobals m;
    ZE_UNUSED(m);

    DeviceOptions opts(default_allocator(), argc, (const char**)argv);
    
    return 0;
}

#endif // ZEAL_PLATFORM_OSX