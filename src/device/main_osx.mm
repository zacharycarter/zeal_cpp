#include "config.h"

#if ZEAL_PLATFORM_OSX

#include "device/device.h"
#include "resource/data_compiler.h"
#import <Cocoa/Cocoa.h>

namespace zeal {} // namespace zeal

struct InitMemoryGlobals {
  InitMemoryGlobals() { zeal::memory_globals::init(); }

  ~InitMemoryGlobals() { zeal::memory_globals::shutdown(); }
};

int main(int argc, const char *const *argv) {
  using namespace zeal;

  InitMemoryGlobals m;
  ZE_UNUSED(m);

  DeviceOptions opts(default_allocator(), argc, (const char **)argv);
  bool quit = false;
  int ec = opts.parse(&quit);

  if (quit)
    return ec;

  if (ec == EXIT_SUCCESS && (opts._do_compile || opts._server)) {
    ec = main_data_compiler(opts);
    if (!opts._do_continue)
      return ec;
  }

  return 0;
}

#endif // ZEAL_PLATFORM_OSX