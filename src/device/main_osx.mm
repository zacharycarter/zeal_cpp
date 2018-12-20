#include "config.h"

#if ZEAL_PLATFORM_OSX

#include "device/device.h"
#include "resource/data_compiler.h"
#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate> {
  bool terminated;
}

+ (AppDelegate *)sharedDelegate;
- (id)init;
- (NSApplicationTerminateReply)applicationShouldTerminate:
    (NSApplication *)sender;
- (bool)applicationHasTerminated;

@end

namespace zeal {
struct MacOSXDevice {
  MacOSXDevice() : m_style(0) {}

  int run(DeviceOptions *opts) {
    [NSApplication sharedApplication];

    id dg = [AppDelegate sharedDelegate];
    [NSApp setDelegate:dg];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp activateIgnoringOtherApps:YES];
    [NSApp finishLaunching];

    [[NSNotificationCenter defaultCenter]
        postNotificationName:NSApplicationWillFinishLaunchingNotification
                      object:NSApp];

    [[NSNotificationCenter defaultCenter]
        postNotificationName:NSApplicationDidFinishLaunchingNotification
                      object:NSApp];

    id quitMenuItem = [NSMenuItem new];
    [quitMenuItem initWithTitle:@"Quit"
                         action:@selector(terminate:)
                  keyEquivalent:@"q"];

    id appMenu = [NSMenu new];
    [appMenu addItem:quitMenuItem];

    id appMenuItem = [NSMenuItem new];
    [appMenuItem setSubmenu:appMenu];

    id menubar = [[NSMenu new] autorelease];
    [menubar addItem:appMenuItem];
    [NSApp setMainMenu:menubar];

    m_style = 0 | NSWindowStyleMaskTitled | NSWindowStyleMaskResizable |
              NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;
    // http://tronche.com/gui/x/xlib/display/XInitThreads.html
    // Status xs = XInitThreads();
    // ZE_ASSERT(xs != 0, "XInitThreads: error");
    // ZE_UNUSED(xs);
    return 0;
  }

  int32_t m_style;
};

static MacOSXDevice m_mdvc;
} // namespace zeal

@implementation AppDelegate

+ (AppDelegate *)sharedDelegate {
  static id delegate = [AppDelegate new];
  return delegate;
}

- (id)init {
  self = [super init];

  if (nil == self) {
    return nil;
  }

  self->terminated = false;
  return self;
}

- (NSApplicationTerminateReply)applicationShouldTerminate:
    (NSApplication *)sender {
  ZE_UNUSED(sender);
  self->terminated = true;
  return NSTerminateCancel;
}

- (bool)applicationHasTerminated {
  return self->terminated;
}

@end

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

  if (ec == EXIT_SUCCESS)
    ec = m_mdvc.run(&opts);

  return ec;
}

#endif // ZEAL_PLATFORM_OSX