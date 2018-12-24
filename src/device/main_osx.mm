#include "config.h"

#if ZEAL_PLATFORM_OSX

#import <Cocoa/Cocoa.h>

#include "core/os.h"
#include "core/thread/thread.h"
#include "device/device.h"
#include "device/display.h"
#include "device/window.h"
#include "resource/data_compiler.h"
#include <bgfx/platform.h>
#include <bx/handlealloc.h>

@interface AppDelegate : NSObject <NSApplicationDelegate> {
  bool terminated;
}

+ (AppDelegate *)sharedDelegate;
- (id)init;
- (NSApplicationTerminateReply)applicationShouldTerminate:
    (NSApplication *)sender;
- (bool)applicationHasTerminated;

@end

@interface CocoaWindow : NSObject <NSWindowDelegate> {
  uint32_t windowCount;
}

+ (CocoaWindow *)sharedDelegate;
- (id)init;
- (void)windowCreated:(NSWindow *)window;
- (void)windowWillClose:(NSNotification *)notification;
- (BOOL)windowShouldClose:(NSWindow *)window;
- (void)windowDidResize:(NSNotification *)notification;
- (void)windowDidBecomeKey:(NSNotification *)notification;
- (void)windowDidResignKey:(NSNotification *)notification;

@end

namespace zeal {
struct WindowHandle {
  u16 idx;
};

static bool s_exit = false;

struct MainThreadArgs {
  DeviceOptions *opts;
};

s32 func(void *data) {
  MainThreadArgs *args = (MainThreadArgs *)data;
  zeal::run(*args->opts);
  s_exit = true;
  return EXIT_SUCCESS;
}

struct MacOSXDevice {
  MacOSXDevice() : _style(0) {}

  NSEvent *peekEvent() {
    return [NSApp
        nextEventMatchingMask:NSEventMaskAny
                    untilDate:[NSDate distantPast] // do not wait for event
                       inMode:NSDefaultRunLoopMode
                      dequeue:YES];
  }

  bool dispatchEvent(NSEvent *event) {
    if (event) {
      [NSApp sendEvent:event];
      [NSApp updateWindows];

      return true;
    }

    return false;
  }

  void windowDidResize(NSWindow *window) {
    WindowHandle handle = handleFromWindow(window);
    NSRect originalFrame = [window frame];
    NSRect rect = [window contentRectForFrameRect:originalFrame];
    uint32_t width = uint32_t(rect.size.width);
    uint32_t height = uint32_t(rect.size.height);
    // m_eventQueue.postSizeEvent(handle, width, height);

    // Make sure mouse button state is 'up' after resize.
    // m_eventQueue.postMouseEvent(handle, m_mx, m_my, m_scroll,
    // MouseButton::Left,
    //                             false);
    // m_eventQueue.postMouseEvent(handle, m_mx, m_my, m_scroll,
    //                             MouseButton::Right, false);
  }

  void windowDidBecomeKey(NSWindow *window) {
    WindowHandle handle = handleFromWindow(window);
    // m_eventQueue.postSuspendEvent(handle, Suspend::WillResume);
    // m_eventQueue.postSuspendEvent(handle, Suspend::DidResume);
  }

  void windowDidResignKey(NSWindow *window) {
    WindowHandle handle = handleFromWindow(window);
    // m_eventQueue.postSuspendEvent(handle, Suspend::WillSuspend);
    // m_eventQueue.postSuspendEvent(handle, Suspend::DidSuspend);
  }

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

    _style = 0 | NSWindowStyleMaskTitled | NSWindowStyleMaskResizable |
             NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;

    NSRect screenRect = [[NSScreen mainScreen] frame];
    const float centerX =
        (screenRect.size.width - (float)ZEAL_DEFAULT_WINDOW_WIDTH) * 0.5f;
    const float centerY =
        (screenRect.size.height - (float)ZEAL_DEFAULT_WINDOW_HEIGHT) * 0.5f;
    // NSString *appName = [[NSProcessInfo processInfo] processName];
    // createWindow(centerX, centerY, ZEAL_DEFAULT_WINDOW_WIDTH,
    //              ZEAL_DEFAULT_WINDOW_HEIGHT, ZEAL_WINDOW_FLAG_NONE,
    //              [appName UTF8String]);

    // Start main thread
    MainThreadArgs mta;
    mta.opts = opts;

    Thread main_thread;
    main_thread.start(func, &mta);

    while (!s_exit) {
      while (dispatchEvent(peekEvent())) {
      }
    }
    return 0;
  }

  bool isValid(WindowHandle _handle) {
    return _windowAlloc.isValid(_handle.idx);
  }

  WindowHandle handleFromWindow(NSWindow *window) {
    uint16_t windowIdx = 0;
    // for (uint16_t i = 0; i < m_windowsCreated; i++) {
    //   if (window == m_window[i]) {
    //     windowIdx = i;
    //     break;
    //   }
    // }
    WindowHandle handle = {windowIdx};
    return handle;
  }

  bx::HandleAllocT<ZEAL_MAX_WINDOWS> _windowAlloc;
  NSWindow *_window[ZEAL_MAX_WINDOWS];
  SInt32 _windowsCreated;
  NSRect _windowFrame;

  int32_t _style;
  Display *_cocoa_display;
  bool _fullscreen;
};

static MacOSXDevice s_mdvc;

void destroyWindow(WindowHandle _handle) {
  if (s_mdvc.isValid(_handle)) {
    dispatch_async(dispatch_get_main_queue(), ^{
      [s_mdvc._window[_handle.idx] performClose:nil];
    });
  }
}

void setWindowPos(WindowHandle _handle, int32_t _x, int32_t _y) {
  if (s_mdvc.isValid(_handle)) {
    NSWindow *window = s_mdvc._window[_handle.idx];
    NSScreen *screen = [window screen];

    NSRect screenRect = [screen frame];
    CGFloat menuBarHeight =
        [[[NSApplication sharedApplication] mainMenu] menuBarHeight];

    NSPoint position = {float(_x),
                        screenRect.size.height - menuBarHeight - float(_y)};

    dispatch_async(dispatch_get_main_queue(), ^{
      [window setFrameTopLeftPoint:position];
    });
  }
}

void setWindowSize(WindowHandle _handle, uint32_t _width, uint32_t _height) {
  if (s_mdvc.isValid(_handle)) {
    NSSize size = {float(_width), float(_height)};
    dispatch_async(dispatch_get_main_queue(), ^{
      [s_mdvc._window[_handle.idx] setContentSize:size];
    });
  }
}

void setWindowTitle(WindowHandle _handle, const char *_title) {
  if (s_mdvc.isValid(_handle)) {
    NSString *title = [[NSString alloc] initWithCString:_title encoding:1];
    dispatch_async(dispatch_get_main_queue(), ^{
      [s_mdvc._window[_handle.idx] setTitle:title];
    });
    [title release];
  }
}

void setWindowFlags(WindowHandle _handle, uint32_t _flags, bool _enabled) {
  ZE_UNUSED(_handle);
  ZE_UNUSED(_flags);
  ZE_UNUSED(_enabled);
}

void toggleFullscreen(WindowHandle _handle) {
  if (s_mdvc.isValid(_handle)) {
    NSWindow *window = s_mdvc._window[_handle.idx];
    NSScreen *screen = [window screen];
    NSRect screenRect = [screen frame];

    if (!s_mdvc._fullscreen) {
      s_mdvc._style &= ~NSWindowStyleMaskTitled;
      dispatch_async(dispatch_get_main_queue(), ^{
        [NSMenu setMenuBarVisible:false];
        [window setStyleMask:s_mdvc._style];
        [window setFrame:screenRect display:YES];
      });

      s_mdvc._fullscreen = true;
    } else {
      s_mdvc._style |= NSWindowStyleMaskTitled;
      dispatch_async(dispatch_get_main_queue(), ^{
        [NSMenu setMenuBarVisible:true];
        [window setStyleMask:s_mdvc._style];
        [window setFrame:s_mdvc._windowFrame display:YES];
      });

      s_mdvc._fullscreen = false;
    }
  }
}

void setMouseLock(WindowHandle _handle, bool _lock) {
  ZE_UNUSED(_handle);
  ZE_UNUSED(_lock);
}

struct WindowCocoa : public Window {
  WindowHandle _cocoa_window;

  WindowCocoa() : _cocoa_window() {}

  void open(u16 x, u16 y, u16 width, u16 height, u32 parent) {
    uint16_t handleIdx = IncrementAtomic(&s_mdvc._windowsCreated);

    if (handleIdx >= ZEAL_MAX_WINDOWS) {
      return;
    }

    _cocoa_window = {handleIdx};

    void (^createWindowBlock)(void) = ^(void) {
      s_mdvc._windowAlloc.alloc();
      NSRect rect = NSMakeRect(x, y, width, height);
      NSWindow *window =
          [[NSWindow alloc] initWithContentRect:rect
                                      styleMask:s_mdvc._style
                                        backing:NSBackingStoreBuffered
                                          defer:NO];
      [window makeKeyAndOrderFront:window];
      [window setAcceptsMouseMovedEvents:YES];
      [window setBackgroundColor:[NSColor blackColor]];
      [[CocoaWindow sharedDelegate] windowCreated:window];

      s_mdvc._window[handleIdx] = window;

      if (s_mdvc._windowsCreated > 1) {
        // s_mdvc.m_eventQueue.postSizeEvent(handle, _width, _height);
        // s_mdvc.m_eventQueue.postWindowEvent(handle, window);
      }
    };

    if ([NSThread isMainThread]) {
      createWindowBlock();
    } else {
      dispatch_async(dispatch_get_main_queue(), createWindowBlock);
    }
  }

  void close(){};

  void bgfx_setup() {
    bgfx::PlatformData pd;
    pd.ndt = NULL;
    pd.nwh = (void *)(uintptr_t)_cocoa_window.idx;
    pd.context = NULL;
    pd.backBuffer = NULL;
    pd.backBufferDS = NULL;
    bgfx::setPlatformData(pd);
  }

  void show() {}

  void hide() {}

  void resize(u16 width, u16 height) {}

  void move(u16 x, u16 y) {}

  void maximize_or_restore(bool maximize) {}

  void minimize() {}

  void maximize() {}

  void restore() {}

  const char *title() {}

  void set_title(const char *title) {}

  void *handle() {}

  void show_cursor(bool show) {}

  void set_fullscreen(bool full) {}
};

namespace window {
Window *create(Allocator &a) { return ZE_NEW(a, WindowCocoa)(); }

void destroy(Allocator &a, Window &w) { ZE_DELETE(a, &w); }

} // namespace window

struct DisplayCocoa : public Display {
  void modes(Array<DisplayMode> &modes) {}

  void set_mode(u32 id) {}
};

namespace display {
Display *create(Allocator &a) { return ZE_NEW(a, DisplayCocoa)(); }

void destroy(Allocator &a, Display &d) { ZE_DELETE(a, &d); }

} // namespace display

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

@implementation CocoaWindow

+ (CocoaWindow *)sharedDelegate {
  static id windowDelegate = [CocoaWindow new];
  return windowDelegate;
}

- (id)init {
  self = [super init];
  if (nil == self) {
    return nil;
  }

  self->windowCount = 0;
  return self;
}

- (void)windowCreated:(NSWindow *)window {
  assert(window);

  [window setDelegate:self];

  assert(self->windowCount < ~0u);
  self->windowCount += 1;
}

- (void)windowWillClose:(NSNotification *)notification {
  ZE_UNUSED(notification);
}

- (BOOL)windowShouldClose:(NSWindow *)window {
  assert(window);

  [window setDelegate:nil];

  assert(self->windowCount);
  self->windowCount -= 1;

  if (self->windowCount == 0) {
    [NSApp terminate:self];
    return false;
  }

  return true;
}

- (void)windowDidResize:(NSNotification *)notification {
  NSWindow *window = [notification object];
  using namespace zeal;
  s_mdvc.windowDidResize(window);
}

- (void)windowDidBecomeKey:(NSNotification *)notification {
  NSWindow *window = [notification object];
  using namespace zeal;
  s_mdvc.windowDidBecomeKey(window);
}

- (void)windowDidResignKey:(NSNotification *)notification {
  NSWindow *window = [notification object];
  using namespace zeal;
  s_mdvc.windowDidResignKey(window);
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
    ec = s_mdvc.run(&opts);

  return ec;
}

#endif // ZEAL_PLATFORM_OSX