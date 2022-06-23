/*
* cocoaGLWindow.m
*
*  Created on: Aug 10, 2015
*      Author: Roger Dass
*		Copyright Frustum Interactive Inc. - All rights reserved.
*/

#import <Cocoa/Cocoa.h>

#include "application/cocoaWrapper.h"

static int mouseLb=0,mouseMb=0,mouseRb=0;

static bool gQuitFlag = false;

void* _glContext = 0;
void* _glPixelFormat = 0;

static int cwNormalKeyCode[256]=
{
	0,                 // 0
	0,                 // 1
	0,                 // 2
	0,                 // 3
	0,                 // 4
	0,                 // 5
	0,                 // 6
	0,                 // 7
	0,                 // 8
	KEY_TAB,         // 9
	0,                 // 10
	0,                 // 11
	0,                 // 12
	KEY_ENTER,       // 13
	0,                 // 14
	0,                 // 15
	0,                 // 16
	0,                 // 17
	0,                 // 18
	0,                 // 19
	0,                 // 20
	0,                 // 21
	0,                 // 22
	0,                 // 23
	0,                 // 24
	0,                 // 25
	0,                 // 26
	KEY_ESC,         // 27
	0,                 // 28
	0,                 // 29
	0,                 // 30
	0,                 // 31
	KEY_SPACE,       // 32
	0,                 // 33
	0,                 // 34
	0,                 // 35
	0,                 // 36
	0,                 // 37
	0,                 // 38
	KEY_SINGLEQUOTE, // 39
	0,                 // 40
	0,                 // 41
	0,                 // 42
	KEY_PLUS,        // 43
	KEY_COMMA,       // 44
	KEY_MINUS,       // 45
	KEY_DOT,         // 46
	KEY_SLASH,       // 47
	KEY_0,           // 48
	KEY_1,           // 49
	KEY_2,           // 50
	KEY_3,           // 51
	KEY_4,           // 52
	KEY_5,           // 53
	KEY_6,           // 54
	KEY_7,           // 55
	KEY_8,           // 56
	KEY_9,           // 57
	KEY_SEMICOLON,   // 58
	0,                 // 59
	0,                 // 60
	0,                 // 61
	0,                 // 62
	0,                 // 63
	0,                 // 64
	KEY_A,           // 65
	KEY_B,           // 66
	KEY_C,           // 67
	KEY_D,           // 68
	KEY_E,           // 69
	KEY_F,           // 70
	KEY_G,           // 71
	KEY_H,           // 72
	KEY_I,           // 73
	KEY_J,           // 74
	KEY_K,           // 75
	KEY_L,           // 76
	KEY_M,           // 77
	KEY_N,           // 78
	KEY_O,           // 79
	KEY_P,           // 80
	KEY_Q,           // 81
	KEY_R,           // 82
	KEY_S,           // 83
	KEY_T,           // 84
	KEY_U,           // 85
	KEY_V,           // 86
	KEY_W,           // 87
	KEY_X,           // 88
	KEY_Y,           // 89
	KEY_Z,           // 90
	KEY_LBRACKET,    // 91
	KEY_BACKSLASH,   // 92
	KEY_RBRACKET,    // 93
	0,                 // 94
	0,                 // 95
	0,                 // 96
	KEY_A,           // 97
	KEY_B,           // 98
	KEY_C,           // 99
	KEY_D,           // 100
	KEY_E,           // 101
	KEY_F,           // 102
	KEY_G,           // 103
	KEY_H,           // 104
	KEY_I,           // 105
	KEY_J,           // 106
	KEY_K,           // 107
	KEY_L,           // 108
	KEY_M,           // 109
	KEY_N,           // 110
	KEY_O,           // 111
	KEY_P,           // 112
	KEY_Q,           // 113
	KEY_R,           // 114
	KEY_S,           // 115
	KEY_T,           // 116
	KEY_U,           // 117
	KEY_V,           // 118
	KEY_W,           // 119
	KEY_X,           // 120
	KEY_Y,           // 121
	KEY_Z,           // 122
	KEY_LBRACKET,    // 123
	0,                 // 124
	KEY_RBRACKET,    // 125
	KEY_TILDA,       // 126
	KEY_BS,          // 127
	0,                 // 128
	0,                 // 129
	0,                 // 130
	0,                 // 131
	0,                 // 132
	0,                 // 133
	0,                 // 134
	0,                 // 135
	0,                 // 136
	0,                 // 137
	0,                 // 138
	0,                 // 139
	0,                 // 140
	0,                 // 141
	0,                 // 142
	0,                 // 143
	0,                 // 144
	0,                 // 145
	0,                 // 146
	0,                 // 147
	0,                 // 148
	0,                 // 149
	0,                 // 150
	0,                 // 151
	0,                 // 152
	0,                 // 153
	0,                 // 154
	0,                 // 155
	0,                 // 156
	0,                 // 157
	0,                 // 158
	0,                 // 159
	0,                 // 160
	0,                 // 161
	0,                 // 162
	0,                 // 163
	0,                 // 164
	0,                 // 165
	0,                 // 166
	0,                 // 167
	0,                 // 168
	0,                 // 169
	0,                 // 170
	0,                 // 171
	0,                 // 172
	0,                 // 173
	0,                 // 174
	0,                 // 175
	0,                 // 176
	0,                 // 177
	0,                 // 178
	0,                 // 179
	0,                 // 180
	0,                 // 181
	0,                 // 182
	0,                 // 183
	0,                 // 184
	0,                 // 185
	0,                 // 186
	0,                 // 187
	0,                 // 188
	0,                 // 189
	0,                 // 190
	0,                 // 191
	0,                 // 192
	0,                 // 193
	0,                 // 194
	0,                 // 195
	0,                 // 196
	0,                 // 197
	0,                 // 198
	0,                 // 199
	0,                 // 200
	0,                 // 201
	0,                 // 202
	0,                 // 203
	0,                 // 204
	0,                 // 205
	0,                 // 206
	0,                 // 207
	0,                 // 208
	0,                 // 209
	0,                 // 210
	0,                 // 211
	0,                 // 212
	0,                 // 213
	0,                 // 214
	0,                 // 215
	0,                 // 216
	0,                 // 217
	0,                 // 218
	0,                 // 219
	0,                 // 220
	0,                 // 221
	0,                 // 222
	0,                 // 223
	0,                 // 224
	0,                 // 225
	0,                 // 226
	0,                 // 227
	0,                 // 228
	0,                 // 229
	0,                 // 230
	0,                 // 231
	0,                 // 232
	0,                 // 233
	0,                 // 234
	0,                 // 235
	0,                 // 236
	0,                 // 237
	0,                 // 238
	0,                 // 239
	0,                 // 240
	0,                 // 241
	0,                 // 242
	0,                 // 243
	0,                 // 244
	0,                 // 245
	0,                 // 246
	0,                 // 247
	0,                 // 248
	0,                 // 249
	0,                 // 250
	0,                 // 251
	0,                 // 252
	0,                 // 253
	0,                 // 254
	0                  // 255
};


static int cwSpecialKeyCode[256]=
{
	KEY_UP,           // 0
	KEY_DOWN,         // 1
	KEY_LEFT,         // 2
	KEY_RIGHT,        // 3
	KEY_F1,           // 4
	KEY_F2,           // 5
	KEY_F3,           // 6
	KEY_F4,           // 7
	KEY_F5,           // 8
	KEY_F6,           // 9
	KEY_F7,           // 10
	KEY_F8,           // 11
	KEY_F9,           // 12
	KEY_F10,          // 13
	KEY_F11,          // 14
	KEY_F12,          // 15
	KEY_F1,           // 16 0x10
	KEY_F2,           // 17
	KEY_F3,           // 18
	KEY_F4,           // 19
	KEY_F5,           // 20
	KEY_F6,           // 21
	KEY_F7,           // 22
	KEY_F8,           // 23
	KEY_F9,           // 24
	KEY_F10,          // 25
	KEY_F11,          // 26
	KEY_F12,          // 27
	KEY_F1,           // 28
	KEY_F2,           // 29
	KEY_F3,           // 30
	KEY_F4,           // 31
	KEY_F5,           // 32 0x20
	KEY_F6,           // 33
	KEY_F7,           // 34
	KEY_F8,           // 35
	KEY_F9,           // 36
	KEY_F10,          // 37
	KEY_F11,          // 38
	KEY_INS,          // 39
	KEY_DEL,          // 40
	KEY_HOME,         // 41
	0,                  // 42 Begin
	KEY_END,          // 43
	KEY_PAGEUP,       // 44
	KEY_PAGEDOWN,     // 45
	KEY_PRINTSCRN,    // 46
	KEY_SCROLLLOCK,   // 47
	KEY_PAUSEBREAK,   // 48 0x30
	0,                  // 49
	KEY_PAUSEBREAK,   // 50
	0,                  // 51 Reset
	0,                  // 52 Stop
	0,                  // 53 Menu
	0,                  // 54 User
	0,                  // 55 System
	KEY_PRINTSCRN,    // 56
	0,                  // 57 Clear line
	0,                  // 58 Clear display
	0,                  // 59 Insert line
	0,                  // 60 Delete line
	KEY_INS,          // 61
	KEY_DEL,          // 62
	KEY_PAGEUP,       // 63
	KEY_PAGEDOWN,     // 64
	0,                  // 65 Select
	0,                  // 66 Execute
	0,                  // 67 Undo
	0,                  // 68 Redo
	0,                  // 69 Find
	0,                  // 70 Help
	0,                  // 71 Mode Switch
	0,                  // 72
	0,                  // 73
	0,                  // 74
	0,                  // 75
	0,                  // 76
	0,                  // 77
	0,                  // 78
	0,                  // 79
	0,                  // 80
	0,                  // 81
	0,                  // 82
	0,                  // 83
	0,                  // 84
	0,                  // 85
	0,                  // 86
	0,                  // 87
	0,                  // 88
	0,                  // 89
	0,                  // 90
	0,                  // 91
	0,                  // 92
	0,                  // 93
	0,                  // 94
	0,                  // 95
	0,                  // 96
	0,                  // 97
	0,                  // 98
	0,                  // 99
	0,                  // 100
	0,                  // 101
	0,                  // 102
	0,                  // 103
	0,                  // 104
	0,                  // 105
	0,                  // 106
	0,                  // 107
	0,                  // 108
	0,                  // 109
	0,                  // 110
	0,                  // 111
	0,                  // 112
	0,                  // 113
	0,                  // 114
	0,                  // 115
	0,                  // 116
	0,                  // 117
	0,                  // 118
	0,                  // 119
	0,                  // 120
	0,                  // 121
	0,                  // 122
	0,                  // 123
	0,                  // 124
	0,                  // 125
	0,                  // 126
	0,                  // 127
	0,                  // 128
	0,                  // 129
	0,                  // 130
	0,                  // 131
	0,                  // 132
	0,                  // 133
	0,                  // 134
	0,                  // 135
	0,                  // 136
	0,                  // 137
	0,                  // 138
	0,                  // 139
	0,                  // 140
	0,                  // 141
	0,                  // 142
	0,                  // 143
	0,                  // 144
	0,                  // 145
	0,                  // 146
	0,                  // 147
	0,                  // 148
	0,                  // 149
	0,                  // 150
	0,                  // 151
	0,                  // 152
	0,                  // 153
	0,                  // 154
	0,                  // 155
	0,                  // 156
	0,                  // 157
	0,                  // 158
	0,                  // 159
	0,                  // 160
	0,                  // 161
	0,                  // 162
	0,                  // 163
	0,                  // 164
	0,                  // 165
	0,                  // 166
	0,                  // 167
	0,                  // 168
	0,                  // 169
	0,                  // 170
	0,                  // 171
	0,                  // 172
	0,                  // 173
	0,                  // 174
	0,                  // 175
	0,                  // 176
	0,                  // 177
	0,                  // 178
	0,                  // 179
	0,                  // 180
	0,                  // 181
	0,                  // 182
	0,                  // 183
	0,                  // 184
	0,                  // 185
	0,                  // 186
	0,                  // 187
	0,                  // 188
	0,                  // 189
	0,                  // 190
	0,                  // 191
	0,                  // 192
	0,                  // 193
	0,                  // 194
	0,                  // 195
	0,                  // 196
	0,                  // 197
	0,                  // 198
	0,                  // 199
	0,                  // 200
	0,                  // 201
	0,                  // 202
	0,                  // 203
	0,                  // 204
	0,                  // 205
	0,                  // 206
	0,                  // 207
	0,                  // 208
	0,                  // 209
	0,                  // 210
	0,                  // 211
	0,                  // 212
	0,                  // 213
	0,                  // 214
	0,                  // 215
	0,                  // 216
	0,                  // 217
	0,                  // 218
	0,                  // 219
	0,                  // 220
	0,                  // 221
	0,                  // 222
	0,                  // 223
	0,                  // 224
	0,                  // 225
	0,                  // 226
	0,                  // 227
	0,                  // 228
	0,                  // 229
	0,                  // 230
	0,                  // 231
	0,                  // 232
	0,                  // 233
	0,                  // 234
	0,                  // 235
	0,                  // 236
	0,                  // 237
	0,                  // 238
	0,                  // 239
	0,                  // 240
	0,                  // 241
	0,                  // 242
	0,                  // 243
	0,                  // 244
	0,                  // 245
	0,                  // 246
	0,                  // 247
	0,                  // 248
	0,                  // 249
	0,                  // 250
	0,                  // 251
	0,                  // 252
	0,                  // 253
	0,                  // 254
	0                   // 255
};


static eKeyCode CWMacUnicodeToKeyCode(int uni)
{
	if(0<=uni && uni<256)
	{
		return cwNormalKeyCode[uni];
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
		//NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
		NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersionLegacy,
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
	if(nKeyBufUsed>0)
	{
		int i;
		eKeyCode kc;
		kc=keyBuffer[0];
		nKeyBufUsed--;
		for(i=0; i<nKeyBufUsed; i++)
		{
			keyBuffer[i]=keyBuffer[i+1];
		}
		return kc;
	}
	return 0;
}

char CWInkeyCharC(void)
{
	if(nCharBufUsed>0)
	{
		int i,c;
		c=charBuffer[0];
		nCharBufUsed--;
		for(i=0; i<nCharBufUsed; i++)
		{
			charBuffer[i]=charBuffer[i+1];
		}
		return c;
	}
	return 0;
}

int CWKeyStateC(eKeyCode kc)
{
	if(0<=kc && kc<KEY_NUM_KEYCODE)
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
