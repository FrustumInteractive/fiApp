#ifndef COCOAWRAPPER_H_
#define COCOAWRAPPER_H_

#include "appDefines.h"

#ifdef __cplusplus
// This needs to be included from Objective-C code for mouse-event enums.
// C++ specific declaration must be enclosed by #ifdef __cplucplus and #endif

void     CWOpenWindow(int x0, int y0, int width, int height, int useDoubleBuffer, float *scaleFactor = nullptr);
void     CWGetWindowSize(int &width, int &height);
void     CWGetScreenSize(int &width, int &height);
void     CWPollDevice(void);
void     CWSleep(int ms);
int      CWPassedTime(void);
void     CWGetMouseState(int *lb,int *mb,int *rb,int *mx,int *my);
int      CWGetMouseEvent(int *lb,int *mb,int *rb,int *mx,int *my);
void     CWSwapBuffers(void);
eKeyCode CWInkey(void);
char     CWInkeyChar(void);
int      CWGetKeyState(eKeyCode);
int      CWCheckWindowExposure(void);
int      CWCheckQuitMessage(void);
void     CWWarpMouseCursorPosition(unsigned x, unsigned y);


void     CWChangeToProgramDir(void); // Mainly for Mac OS X
#endif /* __cplusplus */

#endif /* COCOAWRAPPER_H_ */
