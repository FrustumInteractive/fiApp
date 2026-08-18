#ifndef COCOAWRAPPER_H_
#define COCOAWRAPPER_H_

#include "appDefines.h"

// C-compatible callback used by Cocoa live-resize path.
typedef void (*CWLiveResizeDrawCallback)(void);

#ifdef __cplusplus
// This needs to be included from Objective-C code for mouse-event enums.
// C++ specific declaration must be enclosed by #ifdef __cplucplus and #endif

void     CWOpenWindow(int x0, int y0, int width, int height, int useDoubleBuffer, float *scaleFactor = nullptr);
void     CWGetWindowSize(int &width, int &height);
int      CWConsumeResizeEvent(int *width, int *height);
void     CWGetScreenSize(int &width, int &height);
void     CWPollDevice(void);
void     CWSetLiveResizeDrawCallback(CWLiveResizeDrawCallback cb);
void     CWSleep(int ms);
int      CWPassedTime(void);
void     CWGetMouseState(int *lb,int *mb,int *rb,int *mx,int *my);
int      CWGetMouseEvent(int *lb,int *mb,int *rb,int *mx,int *my,float *dx = nullptr,float *dy = nullptr);
void     CWSwapBuffers(void);
void     CWWaitForDisplayRefresh(void);
eKeyCode CWInkey(void);
char     CWInkeyChar(void);
int      CWGetKeyState(eKeyCode);
int      CWCheckQuitMessage(void);
void     CWWarpMouseCursorPosition(unsigned x, unsigned y);
void     CWWarpMouseCursorPositionInWindow(unsigned x, unsigned y);
void     CWSetVSync(bool enabled);

#if FI_GFX_METAL
void*    CWGetMetalLayer();
void*    CWGetNativeView();
#endif



void     CWChangeToProgramDir(void); // Mainly for Mac OS X
#endif /* __cplusplus */

#endif /* COCOAWRAPPER_H_ */
