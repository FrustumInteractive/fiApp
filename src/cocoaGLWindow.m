/*
* cocoaGLWindow.m
*
*	Created on: Aug 10, 2015
*	Author: Roger Dass
*		Copyright Frustum Interactive Inc. - All rights reserved.
*/

#import <Cocoa/Cocoa.h>
#import <CoreGraphics/CoreGraphics.h>

#include "fi/app/cocoaWrapper.h"
#include "fi/app/cocoaKeyCodes.h"

static int mouseLb=0, mouseMb=0, mouseRb=0;

static bool gQuitFlag = false;

void* _glContext = 0;
void* _glPixelFormat = 0;


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
};


static int cwKeyIsDown[KEY_NUM_KEYCODE]={0};

#define NKEYBUF 256
static int nKeyBufUsed=0;
static int keyBuffer[NKEYBUF];

static int nCharBufUsed=0;
static int charBuffer[NKEYBUF];

static int nMosBufUsed=0;
static struct CWMouseEventLog mosBuffer[NKEYBUF];

static int exposure=0;


@interface CWMacDelegate : NSObject  /*< NSApplicationDelegate > */
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



@interface CWOpenGLWindow : NSWindow
{
}

@end

@implementation CWOpenGLWindow

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
}

- (void) windowWillClose: (NSNotification *)notification
{
	//[NSApp terminate:nil];	// This can also be exit(0);
	gQuitFlag = true;
}

@end


@interface CWOpenGLView : NSOpenGLView 
{
}
- (void) drawRect: (NSRect) bounds;
@end

@implementation CWOpenGLView
-(void) drawRect: (NSRect) bounds
{
	printf("%s\n",__FUNCTION__);
	exposure=1;
}

-(NSMenu *)menuForEvent: (NSEvent *)theEvent
{
	printf("%s\n",__FUNCTION__);
	return [NSView defaultMenu];
}

- (void) flagsChanged: (NSEvent *)theEvent
{
	printf("flagschanged\n");
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
	}
	else if(NKEYBUF>nMosBufUsed)
	{
	  NSRect rect;
	  rect=[self frame];

		mosBuffer[nMosBufUsed].eventType=MOUSEEVENT_MOVE;
		mosBuffer[nMosBufUsed].mx=(int)[theEvent locationInWindow].x;
		mosBuffer[nMosBufUsed].my=rect.size.height-1-(int)[theEvent locationInWindow].y;
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
		mosBuffer[nMosBufUsed].lb=mouseLb;
		mosBuffer[nMosBufUsed].mb=mouseMb;
		mosBuffer[nMosBufUsed].rb=mouseRb;
		nMosBufUsed++;
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

static CWOpenGLWindow *cwWnd=nil;
static CWOpenGLView *cwView=nil;

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
	
	cwWnd=[CWOpenGLWindow alloc];
	[cwWnd
		initWithContentRect:contRect
		styleMask:winStyle
		backing:NSBackingStoreBuffered 
		defer:NO];

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

	if(useDoubleBuffer==0)
	{
	//	formatAttrib[2]=0;
	}

	format=[NSOpenGLPixelFormat alloc];
	[format initWithAttributes: formatAttrib];
	
	cwView=[CWOpenGLView alloc];
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
}

void CWGetWindowSizeC(int *wid,int *hei)
{
	NSRect rect;
	rect=[cwView frame];
	*wid=rect.size.width;
	*hei=rect.size.height;
}

void CWGetScreenSizeC(int *w, int *h)
{
	NSRect screenRect;
	NSArray *screenArray = [NSScreen screens];
	unsigned screenCount = [screenArray count];
	unsigned index  = 0;

	for (index; index < screenCount; index++)
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

int CWGetMouseEventC(int *lb,int *mb,int *rb,int *mx,int *my)
{
	if(0<nMosBufUsed)
	{
		const int eventType=mosBuffer[0].eventType;
		*lb=mosBuffer[0].lb;
		*mb=mosBuffer[0].mb;
		*rb=mosBuffer[0].rb;
		*mx=mosBuffer[0].mx;
		*my=mosBuffer[0].my;

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
		return MOUSEEVENT_NONE;
	}
}

void CWPollDeviceC(void)
{
#ifndef ARC
 	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
#endif

	while(1)
	{
#ifndef ARC
	 	[pool release];
	 	pool=[[NSAutoreleasePool alloc] init];
#endif
	
		NSEvent *event;
		event=[NSApp
			   nextEventMatchingMask:NSEventMaskAny
			   untilDate: [NSDate distantPast]
			   inMode: NSDefaultRunLoopMode
			   dequeue:YES];

		if(event!=nil)
		{
			[NSApp sendEvent:event];
			[NSApp updateWindows];
		}
		else
		{	
			break;
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
	[[cwView openGLContext] makeCurrentContext];
	[[cwView openGLContext] flushBuffer];
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

int CWCheckExposureC(void)
{
	int ret;
	ret=exposure;
	exposure=0;
	return ret;
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
