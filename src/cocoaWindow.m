/*
* cocoaGLWindow.m
*
*	Created on: Aug 10, 2015
*	Author: Roger Dass
*		Copyright Frustum Interactive Inc. - All rights reserved.
*/

#import <Cocoa/Cocoa.h>
#import <CoreGraphics/CoreGraphics.h>
#import <CoreVideo/CoreVideo.h>
#import <QuartzCore/CAMetalLayer.h>
#include <dlfcn.h>
#include <pthread.h>

#include "fi/app/cocoaWrapper.h"
#include "fi/app/cocoaKeyCodes.h"

#pragma clang diagnostic ignored "-Wdeprecated-declarations"

static int mouseLb=0, mouseMb=0, mouseRb=0;

static bool gQuitFlag = false;
static bool gSwapFinishConfigured = false;
static bool gForceFinishBeforeSwap = false;
static bool gGLFinishResolved = false;
static bool gGLFinishAvailable = false;
typedef void (*CWGLFinishProc)(void);
static CWGLFinishProc gGLFinish = 0;

#if !FI_GFX_METAL
static CVDisplayLinkRef gDisplayLink = NULL;
static pthread_mutex_t gDisplayLinkMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t gDisplayLinkCondition = PTHREAD_COND_INITIALIZER;
static uint64_t gDisplayLinkSequence = 0;
static uint64_t gConsumedDisplayLinkSequence = 0;

static CVReturn CWDisplayLinkCallback(
	CVDisplayLinkRef displayLink,
	const CVTimeStamp *now,
	const CVTimeStamp *outputTime,
	CVOptionFlags flagsIn,
	CVOptionFlags *flagsOut,
	void *context)
{
	(void)displayLink;
	(void)now;
	(void)outputTime;
	(void)flagsIn;
	(void)flagsOut;
	(void)context;
	pthread_mutex_lock(&gDisplayLinkMutex);
	++gDisplayLinkSequence;
	pthread_cond_signal(&gDisplayLinkCondition);
	pthread_mutex_unlock(&gDisplayLinkMutex);
	return kCVReturnSuccess;
}

static void CWConfigureDisplayLink(int enabled)
{
	if (!enabled)
	{
		if (gDisplayLink && CVDisplayLinkIsRunning(gDisplayLink))
		{
			CVDisplayLinkStop(gDisplayLink);
		}
		return;
	}

	if (!gDisplayLink)
	{
		if (CVDisplayLinkCreateWithActiveCGDisplays(&gDisplayLink) != kCVReturnSuccess)
		{
			gDisplayLink = NULL;
			return;
		}
		CVDisplayLinkSetOutputCallback(gDisplayLink, CWDisplayLinkCallback, NULL);
	}

	if (!CVDisplayLinkIsRunning(gDisplayLink))
	{
		pthread_mutex_lock(&gDisplayLinkMutex);
		gConsumedDisplayLinkSequence = gDisplayLinkSequence;
		pthread_mutex_unlock(&gDisplayLinkMutex);
		CVDisplayLinkStart(gDisplayLink);
	}
}
#endif

static void CWResolveGLFinishIfNeeded(void)
{
	if (gGLFinishResolved)
	{
		return;
	}

	gGLFinishResolved = true;
#if FI_GFX_METAL
	gGLFinishAvailable = false;
#else
	void *ogl = dlopen("/System/Library/Frameworks/OpenGL.framework/OpenGL", RTLD_LAZY | RTLD_LOCAL);
	if (ogl)
	{
		gGLFinish = (CWGLFinishProc)dlsym(ogl, "glFinish");
		gGLFinishAvailable = (gGLFinish != 0);
	}
#endif
}

void* _glContext = 0;
void* _glPixelFormat = 0;

#if FI_GFX_METAL
	static CAMetalLayer *cwMetalLayer = nil;
#endif


static eKeyCode CWMacUnicodeToKeyCode(int uni)
{
	if(0<=uni && uni<256)
	{
		return cwKeyCode[uni];
	}
	else if(0xf700<=uni && uni<0xf800)
	{
		return cwSpecialKeyCode[uni-0xf700];
	}
	return KEY_NULL;
}

struct CWMouseEventLog
{
	int eventType;
	int lb,mb,rb;
	int mx,my;
	float dx,dy;
};


static int cwKeyIsDown[KEY_NUM_KEYCODE]={0};

#define NKEYBUF 256
static int nKeyBufUsed=0;
static int keyBuffer[NKEYBUF];

static int nCharBufUsed=0;
static int charBuffer[NKEYBUF];

static int nMosBufUsed=0;
static struct CWMouseEventLog mosBuffer[NKEYBUF];

static bool gResizePending = false;
static int gResizeWidth = 0;
static int gResizeHeight = 0;
static int gLastObservedWidth = 0;
static int gLastObservedHeight = 0;
static CWLiveResizeDrawCallback gLiveResizeDrawCallback = 0;


@interface CWMacDelegate : NSObject <NSApplicationDelegate>
/* Example: Fire has the same problem no explanation */
{
}
/* - (BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication; */
@end

@implementation CWMacDelegate

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
	return NO;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	// Insert code here to initialize your application
	printf("finished launching...\n");
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
	printf("application will terminate...\n");
	// Insert code here to tear down your application
}
@end



@interface CWWindow : NSWindow
{
}
@end

#if FI_GFX_METAL
	@interface CWView : NSView {}
#else
	@interface CWView : NSOpenGLView {}
#endif
- (void) drawRect: (NSRect) bounds;
@end

static CWWindow *cwWnd=nil;
static CWView *cwView=nil;

@implementation CWWindow

- (id) initWithContentRect: (NSRect)rect
				styleMask:(NSWindowStyleMask)wndStyle
				   backing:(NSBackingStoreType)bufferingType
					 defer:(BOOL)deferFlg
{
	self = [super initWithContentRect:rect styleMask:wndStyle backing:bufferingType defer:deferFlg];

	if(self != nil)
	{
		[[NSNotificationCenter defaultCenter]
			addObserver:self
			selector:@selector(windowDidResize:)
			name:NSWindowDidResizeNotification
			object:self];
		
		[[NSNotificationCenter defaultCenter]
			addObserver:self
			selector:@selector(windowWillClose:)
			name:NSWindowWillCloseNotification
			object:self];
		
		[self setAcceptsMouseMovedEvents:YES];
	}
	
	return self;
}

- (void) windowDidResize: (NSNotification *)notification
{
	if(cwView != nil)
	{
		NSRect rect = [cwView frame];
		gResizeWidth = (int)rect.size.width;
		gResizeHeight = (int)rect.size.height;
		gResizePending = true;
	}

#if FI_GFX_METAL
	if (!cwView || !cwMetalLayer) return;
	NSRect bounds = [cwView bounds];
	CGFloat s = [[self screen] backingScaleFactor];
	cwMetalLayer.contentsScale = s;
	cwMetalLayer.drawableSize = CGSizeMake(bounds.size.width * s, bounds.size.height * s);
#else
	if(cwView != nil)
	{
		NSOpenGLContext *ctx = [cwView openGLContext];
		if(ctx != nil)
		{
			[ctx update];
		}
	}
#endif
}

- (void) windowWillClose: (NSNotification *)notification
{
	//[NSApp terminate:nil];	// This can also be exit(0);
	gQuitFlag = true;
}

@end



@implementation CWView
-(BOOL) preservesContentDuringLiveResize
{
	// Force real redraws during interactive resize instead of stretched old framebuffer.
	return NO;
}

-(void) reshape
{
	[super reshape];

	// AppKit calls this during live resize. Capture the latest content size here so
	// the app can react immediately instead of waiting for deferred notifications.
	if(cwView != nil)
	{
		NSRect rect = [cwView bounds];
		gResizeWidth = (int)rect.size.width;
		gResizeHeight = (int)rect.size.height;
		gResizePending = true;
	}

#if !FI_GFX_METAL
	NSOpenGLContext *ctx = [self openGLContext];
	if(ctx != nil)
	{
		[ctx update];
	}
#endif
}

-(void) drawRect: (NSRect) bounds
{
	if(cwWnd != nil && [cwWnd inLiveResize] && gLiveResizeDrawCallback != 0)
	{
		gLiveResizeDrawCallback();
	}
}

-(NSMenu *)menuForEvent: (NSEvent *)theEvent
{
	printf("%s\n",__FUNCTION__);
	return [NSView defaultMenu];
}

- (void) flagsChanged: (NSEvent *)theEvent
{
	//printf("flagschanged\n");
	unsigned int flags;
	flags=[theEvent modifierFlags];

	if(flags&NSEventModifierFlagCapsLock) // Caps
	{
		if(cwKeyIsDown[KEY_CAPSLOCK]==0 && nKeyBufUsed<NKEYBUF)
		{
			keyBuffer[nKeyBufUsed++]=KEY_CAPSLOCK;
		}
		cwKeyIsDown[KEY_CAPSLOCK]=1;
	}
	else
	{
		cwKeyIsDown[KEY_CAPSLOCK]=0;
	}

	if(flags&NSEventModifierFlagShift)
	{
		if(cwKeyIsDown[KEY_SHIFT]==0 && nKeyBufUsed<NKEYBUF)
		{
			keyBuffer[nKeyBufUsed++]=KEY_SHIFT;
		}
		cwKeyIsDown[KEY_SHIFT]=1;
	}
	else
	{
		cwKeyIsDown[KEY_SHIFT]=0;
	}

	if(flags&NSEventModifierFlagControl)
	{
		if(cwKeyIsDown[KEY_CTRL]==0 && nKeyBufUsed<NKEYBUF)
		{
			keyBuffer[nKeyBufUsed++]=KEY_CTRL;
		}
		cwKeyIsDown[KEY_CTRL]=1;
	}
	else
	{
		cwKeyIsDown[KEY_CTRL]=0;
	}

	if((flags&NSEventModifierFlagOption) || (flags&NSEventModifierFlagCommand))
	{
		if(cwKeyIsDown[KEY_ALT]==0 && nKeyBufUsed<NKEYBUF)
		{
			keyBuffer[nKeyBufUsed++]=KEY_ALT;
		}
		cwKeyIsDown[KEY_ALT]=1;
	}
	else
	{
		cwKeyIsDown[KEY_ALT]=0;
	}

	// Other possible key masks
	// NSNumericPadKeyMask
	// NSHelpKeyMask
	// NSFunctionKeyMask
	// NSDeviceIndependentModifierFlagsMask
}

- (void) keyDown:(NSEvent *)theEvent
{
	unsigned int flags;
	flags=[theEvent modifierFlags];

	NSString *chrs,*chrsNoMod;
	chrs=[theEvent characters];
	if(0==(flags & NSEventModifierFlagCommand) && [chrs length]>0)
	{
		int unicode;
		unicode=[chrs characterAtIndex:0];

		if(32<=unicode && unicode<128 && nCharBufUsed<NKEYBUF)
		{
			charBuffer[nCharBufUsed++]=unicode;
		}
	}

	chrsNoMod=[theEvent charactersIgnoringModifiers];
	if([chrsNoMod length]>0)
	{
		int unicode;
		eKeyCode kc;
		unicode=[chrsNoMod characterAtIndex:0];
		kc=CWMacUnicodeToKeyCode(unicode);

		if(kc!=0)
		{
			cwKeyIsDown[kc]=1;

			if(nKeyBufUsed<NKEYBUF)
			{
				keyBuffer[nKeyBufUsed++]=kc;
			}
		}
	}
}

- (void) keyUp:(NSEvent *)theEvent
{
	NSString *chrs,*chrsNoMod;
	chrs=[theEvent characters];
	if([chrs length]>0)
	{
		int unicode;
		unicode=[chrs characterAtIndex:0];
	}

	chrsNoMod=[theEvent charactersIgnoringModifiers];
	if([chrsNoMod length]>0)
	{
		int unicode;
		eKeyCode kc;
		unicode=[chrsNoMod characterAtIndex:0];
		kc=CWMacUnicodeToKeyCode(unicode);

		if(kc!=0)
		{
			cwKeyIsDown[kc]=0;

			if(nKeyBufUsed<NKEYBUF)
			{
				keyBuffer[nKeyBufUsed++]=kc;
			}
		}
	}
}

- (void) mouseMoved:(NSEvent *)theEvent
{
	if(0<nMosBufUsed && MOUSEEVENT_MOVE==mosBuffer[nMosBufUsed-1].eventType)
	{
	  NSRect rect;
	  rect=[self frame];

		mosBuffer[nMosBufUsed-1].mx=(int)[theEvent locationInWindow].x;
		mosBuffer[nMosBufUsed-1].my=rect.size.height-1-(int)[theEvent locationInWindow].y;
		mosBuffer[nMosBufUsed-1].dx += (float)[theEvent deltaX];
		mosBuffer[nMosBufUsed-1].dy += (float)[theEvent deltaY];
	}
	else if(NKEYBUF>nMosBufUsed)
	{
	  NSRect rect;
	  rect=[self frame];

		mosBuffer[nMosBufUsed].eventType=MOUSEEVENT_MOVE;
		mosBuffer[nMosBufUsed].mx=(int)[theEvent locationInWindow].x;
		mosBuffer[nMosBufUsed].my=rect.size.height-1-(int)[theEvent locationInWindow].y;
		mosBuffer[nMosBufUsed].dx=(float)[theEvent deltaX];
		mosBuffer[nMosBufUsed].dy=(float)[theEvent deltaY];
		mosBuffer[nMosBufUsed].lb=mouseLb;
		mosBuffer[nMosBufUsed].mb=mouseMb;
		mosBuffer[nMosBufUsed].rb=mouseRb;
		nMosBufUsed++;
	}
}

- (void) mouseDragged:(NSEvent *)theEvent
{
  [self mouseMoved:theEvent];
}
/*
- (void) rightMouseDragged:(NSEvent *)theEvent
{
  [self mouseMoved:theEvent];
}
*/
- (void) otherMouseDragged:(NSEvent *)theEvent
{
  [self mouseMoved:theEvent];
}

- (void) mouseDown:(NSEvent *)theEvent
{
	mouseLb=1;

	if(NKEYBUF>nMosBufUsed)
	{
		NSRect rect;
		rect=[self frame];

		mosBuffer[nMosBufUsed].eventType=MOUSEEVENT_LBUTTONDOWN;
		mosBuffer[nMosBufUsed].mx=(int)[theEvent locationInWindow].x;
		mosBuffer[nMosBufUsed].my=rect.size.height-1-(int)[theEvent locationInWindow].y;
		mosBuffer[nMosBufUsed].dx=0.0f;
		mosBuffer[nMosBufUsed].dy=0.0f;
		mosBuffer[nMosBufUsed].lb=mouseLb;
		mosBuffer[nMosBufUsed].mb=mouseMb;
		mosBuffer[nMosBufUsed].rb=mouseRb;
		nMosBufUsed++;
	}
}

- (void) mouseUp:(NSEvent *)theEvent
{
	mouseLb=0;

	if(NKEYBUF>nMosBufUsed)
	{
		NSRect rect;
		rect=[self frame];

		mosBuffer[nMosBufUsed].eventType=MOUSEEVENT_LBUTTONUP;
		mosBuffer[nMosBufUsed].mx=(int)[theEvent locationInWindow].x;
		mosBuffer[nMosBufUsed].my=rect.size.height-1-(int)[theEvent locationInWindow].y;
		mosBuffer[nMosBufUsed].dx=0.0f;
		mosBuffer[nMosBufUsed].dy=0.0f;
		mosBuffer[nMosBufUsed].lb=mouseLb;
		mosBuffer[nMosBufUsed].mb=mouseMb;
		mosBuffer[nMosBufUsed].rb=mouseRb;
		nMosBufUsed++;
	}
}

- (void) rightMouseDown:(NSEvent *)theEvent
{
	mouseRb=1;

	if(NKEYBUF>nMosBufUsed)
	{
		NSRect rect;
		rect=[self frame];

		mosBuffer[nMosBufUsed].eventType=MOUSEEVENT_RBUTTONDOWN;
		mosBuffer[nMosBufUsed].mx=(int)[theEvent locationInWindow].x;
		mosBuffer[nMosBufUsed].my=rect.size.height-1-(int)[theEvent locationInWindow].y;
		mosBuffer[nMosBufUsed].dx=0.0f;
		mosBuffer[nMosBufUsed].dy=0.0f;
		mosBuffer[nMosBufUsed].lb=mouseLb;
		mosBuffer[nMosBufUsed].mb=mouseMb;
		mosBuffer[nMosBufUsed].rb=mouseRb;
		nMosBufUsed++;
	}
}

- (void) rightMouseUp:(NSEvent *)theEvent
{
	mouseRb=0;

	if(NKEYBUF>nMosBufUsed)
	{
		NSRect rect;
		rect=[self frame];

		mosBuffer[nMosBufUsed].eventType=MOUSEEVENT_RBUTTONUP;
		mosBuffer[nMosBufUsed].mx=(int)[theEvent locationInWindow].x;
		mosBuffer[nMosBufUsed].my=rect.size.height-1-(int)[theEvent locationInWindow].y;
		mosBuffer[nMosBufUsed].dx=0.0f;
		mosBuffer[nMosBufUsed].dy=0.0f;
		mosBuffer[nMosBufUsed].lb=mouseLb;
		mosBuffer[nMosBufUsed].mb=mouseMb;
		mosBuffer[nMosBufUsed].rb=mouseRb;
		nMosBufUsed++;
	}
}

- (void) otherMouseDown:(NSEvent *)theEvent
{
	mouseMb=1;

	if(NKEYBUF>nMosBufUsed)
	{
		NSRect rect;
		rect=[self frame];

		mosBuffer[nMosBufUsed].eventType=MOUSEEVENT_MBUTTONDOWN;
		mosBuffer[nMosBufUsed].mx=(int)[theEvent locationInWindow].x;
		mosBuffer[nMosBufUsed].my=rect.size.height-1-(int)[theEvent locationInWindow].y;
		mosBuffer[nMosBufUsed].dx=0.0f;
		mosBuffer[nMosBufUsed].dy=0.0f;
		mosBuffer[nMosBufUsed].lb=mouseLb;
		mosBuffer[nMosBufUsed].mb=mouseMb;
		mosBuffer[nMosBufUsed].rb=mouseRb;
		nMosBufUsed++;
	}
}

- (void) otherMouseUp:(NSEvent *)theEvent
{
	mouseMb=0;

	if(NKEYBUF>nMosBufUsed)
	{
		NSRect rect;
		rect=[self frame];

		mosBuffer[nMosBufUsed].eventType=MOUSEEVENT_MBUTTONUP;
		mosBuffer[nMosBufUsed].mx=(int)[theEvent locationInWindow].x;
		mosBuffer[nMosBufUsed].my=rect.size.height-1-(int)[theEvent locationInWindow].y;
		mosBuffer[nMosBufUsed].dx=0.0f;
		mosBuffer[nMosBufUsed].dy=0.0f;
		mosBuffer[nMosBufUsed].lb=mouseLb;
		mosBuffer[nMosBufUsed].mb=mouseMb;
		mosBuffer[nMosBufUsed].rb=mouseRb;
		nMosBufUsed++;
	}
}

- (void) scrollWheel:(NSEvent *)theEvent
{
	CGFloat dy = [theEvent scrollingDeltaY];
	if (dy > 0.0)
	{
		if (nKeyBufUsed < NKEYBUF) keyBuffer[nKeyBufUsed++] = KEY_WHEELUP;
	}
	else if (dy < 0.0)
	{
		if (nKeyBufUsed < NKEYBUF) keyBuffer[nKeyBufUsed++] = KEY_WHEELDOWN;
	}
}

/*
- (BOOL)acceptsFirstResponder
{
    return YES;
}
*/
@end



void CWAddMenu(void)
{
#ifndef ARC
 	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
#endif

	NSMenu *mainMenu;

	mainMenu=[NSMenu alloc];
	[mainMenu initWithTitle:@"Minimum"];

	NSMenuItem *fileMenu;
	fileMenu=[[NSMenuItem alloc] initWithTitle:@"File" action:NULL keyEquivalent:[NSString string]];
	[mainMenu addItem:fileMenu];

	NSMenu *fileSubMenu;
	fileSubMenu=[[NSMenu alloc] initWithTitle:@"File"];
	[fileMenu setSubmenu:fileSubMenu];

	NSMenuItem *fileMenu_Quit;
	fileMenu_Quit=[[NSMenuItem alloc] initWithTitle:@"Quit"  action:@selector(terminate:) keyEquivalent:@"q"];
	[fileMenu_Quit setTarget:NSApp];
	[fileSubMenu addItem:fileMenu_Quit];

	[NSApp setMainMenu:mainMenu];

#ifndef ARC
	[pool release];
#endif
}

void CWTestApplicationPath(void)
{
#ifndef ARC
 	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
#endif

	char cwd[256];
	getcwd(cwd,255);
	printf("CWD(Initial): %s\n",cwd);

	NSString *path;
	path=[[NSBundle mainBundle] bundlePath];
	printf("BundlePath:%s\n",[path UTF8String]);

	[[NSFileManager defaultManager] changeCurrentDirectoryPath:path];

	getcwd(cwd,255);
	printf("CWD(Changed): %s\n",cwd);
	
#ifndef ARC
	[pool release];
#endif
}

/* ******************************************
 *
 *  OpenGL stuff - all deprecated now on mac
 *
 ********************************************/

void CWOpenWindowC(int x0,int y0,int wid,int hei,int useDoubleBuffer, float *scaleFactor)
{
#ifndef ARC
 	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
#endif

	NSArray *tl;
	NSApplication *application = [NSApplication sharedApplication];

	CWMacDelegate *delegate	= [[CWMacDelegate alloc] init];
	[application setDelegate: delegate];
	[application finishLaunching];

	NSRect contRect;
	contRect=NSMakeRect(x0,y0,wid,hei);
	
	unsigned int winStyle=
		NSWindowStyleMaskTitled|
		NSWindowStyleMaskClosable|
		NSWindowStyleMaskMiniaturizable|
		NSWindowStyleMaskResizable;
	
	cwWnd=[CWWindow alloc];
	[cwWnd
		initWithContentRect:contRect
		styleMask:winStyle
		backing:NSBackingStoreBuffered 
		defer:NO];

#if FI_GFX_METAL
	cwView = [CWView alloc];
	contRect = NSMakeRect(0,0,wid,hei);
	[cwView initWithFrame:contRect];

	[cwView setWantsLayer:YES];
	cwMetalLayer = [CAMetalLayer layer];
	[cwView setLayer:cwMetalLayer];

	CGFloat s = [[cwWnd screen] backingScaleFactor];
	if (scaleFactor) *scaleFactor = (float)s;

	cwMetalLayer.contentsScale = s;
	cwMetalLayer.drawableSize = CGSizeMake(wid * s, hei * s);
#else
	NSOpenGLPixelFormat *format;
	NSOpenGLPixelFormatAttribute formatAttrib[]=
	{
		NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
		//NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersionLegacy,
		NSOpenGLPFADepthSize, 24,
		NSOpenGLPFAColorSize, 24,
		NSOpenGLPFAAlphaSize, 8,
		NSOpenGLPFAAccelerated,
		0
	};

	format=[NSOpenGLPixelFormat alloc];
	[format initWithAttributes: formatAttrib];
	
	cwView=[CWView alloc];
	contRect=NSMakeRect(0,0,wid,hei);
	[cwView	initWithFrame:contRect];
	[cwView setPixelFormat:format];
	[[cwView openGLContext] makeCurrentContext];

	if(scaleFactor) {
		NSRect screenRect = [cwView convertRectToBacking: contRect];
		*scaleFactor = screenRect.size.width / contRect.size.width;
	}

	_glContext = [[cwView openGLContext] CGLContextObj];
	_glPixelFormat = [format CGLPixelFormatObj];
#endif
	
	[cwWnd setContentView:cwView];
	[cwWnd makeFirstResponder:cwView];

	[cwWnd makeKeyAndOrderFront:nil];
	[cwWnd makeMainWindow];

	[application activateIgnoringOtherApps:YES];

	CWAddMenu();

#ifndef ARC
	[pool release];
#endif

	int i;
	for(i=0; i<KEY_NUM_KEYCODE; i++)
	{
		cwKeyIsDown[i]=0;
	}

	// Seed resize tracking with initial content size.
#if FI_GFX_METAL
	NSRect b = [cwView bounds];
	gLastObservedWidth = (int)b.size.width;
	gLastObservedHeight = (int)b.size.height;
#else
	NSRect b = [cwView bounds];
	gLastObservedWidth = (int)b.size.width;
	gLastObservedHeight = (int)b.size.height;
#endif
	gResizeWidth = gLastObservedWidth;
	gResizeHeight = gLastObservedHeight;
}

void CWGetWindowSizeC(int *wid,int *hei)
{
#if FI_GFX_METAL
	CGSize ds = cwMetalLayer.drawableSize; // pixels
	*wid = (int)ds.width;
	*hei = (int)ds.height;
#else
	NSRect rect;
	rect=[cwView bounds];
	*wid=rect.size.width;
	*hei=rect.size.height;
#endif
}

int CWConsumeResizeEventC(int *wid,int *hei)
{
	if(gResizePending)
	{
		gResizePending = false;
		*wid = gResizeWidth;
		*hei = gResizeHeight;
		return 1;
	}
	return 0;
}

void CWSetLiveResizeDrawCallbackC(CWLiveResizeDrawCallback cb)
{
	gLiveResizeDrawCallback = cb;
}

void CWGetScreenSizeC(int *w, int *h)
{
	NSRect screenRect;
	NSArray *screenArray = [NSScreen screens];
	unsigned screenCount = [screenArray count];
	unsigned index  = 0;

	for (; index < screenCount; index++)
	{
		NSScreen *screen = [screenArray objectAtIndex: index];
		screenRect = [screen visibleFrame];
	}
	*w = screenRect.size.width;
	*h = screenRect.size.height;
}

void CWMouseC(int *lb,int *mb,int *rb,int *mx,int *my)
{
	*lb=mouseLb;
	*mb=mouseMb;
	*rb=mouseRb;

	NSPoint loc;
	loc=[NSEvent mouseLocation];
	//loc=[cwWnd convertScreenToBase:loc];
	//loc=[cwView convertPointFromBase:loc];

	NSRect rect;
	rect=[cwView frame];
	*mx=loc.x;
	*my=rect.size.height-1-loc.y;
}

int CWGetMouseEventC(int *lb,int *mb,int *rb,int *mx,int *my,float *dx,float *dy)
{
	if(0<nMosBufUsed)
	{
		const int eventType=mosBuffer[0].eventType;
		*lb=mosBuffer[0].lb;
		*mb=mosBuffer[0].mb;
		*rb=mosBuffer[0].rb;
		*mx=mosBuffer[0].mx;
		*my=mosBuffer[0].my;
		*dx=mosBuffer[0].dx;
		*dy=mosBuffer[0].dy;

		int i;
		for(i=0; i<nMosBufUsed-1; i++)
		{
			mosBuffer[i]=mosBuffer[i+1];
		}

		nMosBufUsed--;
		return eventType;
	}
	else
	{
		CWMouseC(lb,mb,rb,mx,my);
		*dx=0.0f;
		*dy=0.0f;
		return MOUSEEVENT_NONE;
	}
}

void CWPollDeviceC(void)
{
#ifndef ARC
 	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
#endif

	// Keep event handling responsive but bounded per frame so rendering can continue
	// during heavy input streams (for example live window resize).
	const int kMaxEventsPerPoll = 64;
	int processed = 0;
	while(processed < kMaxEventsPerPoll)
	{
#ifndef ARC
	 	[pool release];
	 	pool=[[NSAutoreleasePool alloc] init];
#endif

		NSEvent *event = [NSApp
			nextEventMatchingMask:NSEventMaskAny
			untilDate:[NSDate distantPast]
			inMode:NSDefaultRunLoopMode
			dequeue:YES];

		if(event == nil)
		{
			// During live window resize, AppKit posts events in tracking mode.
			// Pump this mode too so rendering/layout/input continue while dragging.
			event = [NSApp
				nextEventMatchingMask:NSEventMaskAny
				untilDate:[NSDate distantPast]
				inMode:NSEventTrackingRunLoopMode
				dequeue:YES];
		}

		if(event == nil)
		{
			// Also service modal-loop events when panels/menus are active.
			event = [NSApp
				nextEventMatchingMask:NSEventMaskAny
				untilDate:[NSDate distantPast]
				inMode:NSModalPanelRunLoopMode
				dequeue:YES];
		}

		if(event == nil)
		{
			break;
		}

		[NSApp sendEvent:event];
		[NSApp updateWindows];
		processed++;
	}

	// Fallback resize detection for live-resize paths where notifications may be
	// delayed/coalesced. This keeps the app resize callback in sync per frame.
	if(cwView != nil)
	{
		NSRect rect = [cwView bounds];
		int w = (int)rect.size.width;
		int h = (int)rect.size.height;
		if(w > 0 && h > 0 && (w != gLastObservedWidth || h != gLastObservedHeight))
		{
			gLastObservedWidth = w;
			gLastObservedHeight = h;
			gResizeWidth = w;
			gResizeHeight = h;
			gResizePending = true;
		}
	}
#ifndef ARC
	[pool release];
#endif
}

void CWSleepC(int ms)
{
	if(ms>0)
	{
		double sec;
		sec=(double)ms/1000.0;
		[NSThread sleepForTimeInterval:sec];
	}
}

int CWPassedTimeC(void)
{
	int ms;

#ifndef ARC
 	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
#endif

	static NSTimeInterval last=0.0;
	NSTimeInterval now;

	now=[[NSDate date] timeIntervalSince1970];

	NSTimeInterval passed;
	passed=now-last;
	ms=(int)(1000.0*passed);

	if(ms<0)
	{
		ms=1;
	}
	last=now;

#ifndef ARC
	[pool release];
#endif

	return ms;
}

void CWSwapBufferC(void)
{
#if FI_GFX_METAL
	// no-op for metal/vulkan
#else
	if (!gSwapFinishConfigured)
	{
		const char* e = getenv("FI_GL_SWAP_FORCE_FINISH");
		gForceFinishBeforeSwap = (e && atoi(e) != 0);
		gSwapFinishConfigured = true;
		printf("OpenGL swap diagnostics: FI_GL_SWAP_FORCE_FINISH=%s (%s)\n",
			e ? e : "unset",
			gForceFinishBeforeSwap ? "enabled" : "disabled");
	}

	[[cwView openGLContext] makeCurrentContext];
	if (gForceFinishBeforeSwap)
	{
		CWResolveGLFinishIfNeeded();
		if (gGLFinishAvailable)
		{
			gGLFinish();
		}
		else
		{
			static bool once = false;
			if (!once)
			{
				once = true;
				printf("OpenGL swap diagnostics: glFinish unavailable; skipping forced finish\n");
			}
		}
	}
	[[cwView openGLContext] flushBuffer];
#endif
}

void CWWaitForDisplayRefreshC(void)
{
#if !FI_GFX_METAL
	if (!gDisplayLink || !CVDisplayLinkIsRunning(gDisplayLink))
	{
		return;
	}

	pthread_mutex_lock(&gDisplayLinkMutex);
	while (gDisplayLinkSequence == gConsumedDisplayLinkSequence)
	{
		pthread_cond_wait(&gDisplayLinkCondition, &gDisplayLinkMutex);
	}
	gConsumedDisplayLinkSequence = gDisplayLinkSequence;
	pthread_mutex_unlock(&gDisplayLinkMutex);
#endif
}

void CWSetVSyncC(int enabled)
{
#if FI_GFX_METAL
	(void)enabled;
#else
	[[cwView openGLContext] makeCurrentContext];
	GLint swapInterval = enabled ? 1 : 0;
	[[cwView openGLContext] setValues:&swapInterval forParameter:NSOpenGLContextParameterSwapInterval];
	CWConfigureDisplayLink(enabled);

	GLint readback = -1;
	[[cwView openGLContext] getValues:&readback forParameter:NSOpenGLContextParameterSwapInterval];

	double hz = 0.0;
	if (cwWnd)
	{
		NSScreen *screen = [cwWnd screen];
		NSNumber *screenNum = [[screen deviceDescription] objectForKey:@"NSScreenNumber"];
		if (screenNum)
		{
			CGDisplayModeRef mode = CGDisplayCopyDisplayMode((CGDirectDisplayID)[screenNum unsignedIntValue]);
			if (mode)
			{
				hz = CGDisplayModeGetRefreshRate(mode);
				CGDisplayModeRelease(mode);
			}
		}
	}

	printf("OpenGL vsync set request=%d readback=%d display_refresh=%.3fHz\n", enabled ? 1 : 0, (int)readback, hz);
#endif
}

eKeyCode CWInkeyC(void)
{
	if (nKeyBufUsed>0)
	{
		int i;
		eKeyCode kc;
		kc=keyBuffer[0];
		nKeyBufUsed--;
		for (i=0; i<nKeyBufUsed; i++)
		{
			keyBuffer[i]=keyBuffer[i+1];
		}
		return kc;
	}
	return 0;
}

char CWInkeyCharC(void)
{
	if (nCharBufUsed>0)
	{
		int i,c;
		c=charBuffer[0];
		nCharBufUsed--;
		for (i=0; i<nCharBufUsed; i++)
		{
			charBuffer[i]=charBuffer[i+1];
		}
		return c;
	}
	return 0;
}

int CWKeyStateC(eKeyCode kc)
{
	if (0<=kc && kc<KEY_NUM_KEYCODE)
	{
		return cwKeyIsDown[kc];
	}
	return 0;
}

void CWChangeToProgramDirC(void)
{
	NSString *path;
	path=[[NSBundle mainBundle] bundlePath];
	printf("BundlePath:%s\n",[path UTF8String]);

	[[NSFileManager defaultManager] changeCurrentDirectoryPath:path];
}

int CWCheckQuitMessageC(void)
{
	if( gQuitFlag )
		return 1;
	else return 0;
}

void CWWarpMouseCursorPositionC(unsigned x, unsigned y)
{
	CGPoint p;
	p.x = x;
	p.y = y;
	CGWarpMouseCursorPosition(p);
}

void CWWarpMouseCursorPositionInWindowC(unsigned x, unsigned y)
{
	if(nil==cwWnd || nil==cwView)
	{
		return;
	}

	NSRect bounds = [cwView bounds];
	NSPoint viewPoint = NSMakePoint((CGFloat)x, bounds.size.height - 1.0 - (CGFloat)y);
	NSPoint windowPoint = [cwView convertPoint:viewPoint toView:nil];
	NSPoint screenPoint = [cwWnd convertPointToScreen:windowPoint];

	NSScreen *screen = [cwWnd screen];
	if(nil==screen)
	{
		screen = [NSScreen mainScreen];
	}
	NSRect screenFrame = [screen frame];
	CGPoint p;
	p.x = screenPoint.x - screenFrame.origin.x;
	p.y = screenFrame.origin.y + screenFrame.size.height - screenPoint.y;
	if(p.x < 0.0) p.x = 0.0;
	if(p.y < 0.0) p.y = 0.0;
	if(p.x > screenFrame.size.width - 1.0) p.x = screenFrame.size.width - 1.0;
	if(p.y > screenFrame.size.height - 1.0) p.y = screenFrame.size.height - 1.0;
	CGWarpMouseCursorPosition(p);
}

#if FI_GFX_METAL
void* CWGetMetalLayerC(void) { return (void*)cwMetalLayer; }
void* CWGetNativeViewC(void) { return (void*)cwView; }
#endif
