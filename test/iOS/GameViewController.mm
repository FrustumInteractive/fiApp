//
//  GameViewController.m
//  fiViewer
//
//  Created by Roger Dass on 2016-02-25.
//  Copyright © 2016 Frustum Interactive. All rights reserved.
//

#import "GameViewController.h"
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

@interface GameViewController () {
    MyApp _app;
    float _scaleFactor;

	BOOL isFirstDraw;
}
@property (strong, nonatomic) EAGLContext *context;
@property (strong, nonatomic) GLKBaseEffect *effect;

- (void)setupGL;
- (void)tearDownGL;

//- (BOOL)loadShaders;
//- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file;
//- (BOOL)linkProgram:(GLuint)prog;
//- (BOOL)validateProgram:(GLuint)prog;
@end

@implementation GameViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    //self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
	self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    // get native scale factor for iphone 6S plus
    _scaleFactor = view.contentScaleFactor = [UIScreen mainScreen].nativeScale;

	// set framerate
	self.preferredFramesPerSecond = 60;
    
    NSString *executiblePath = [[[NSBundle mainBundle] bundlePath] stringByAppendingString:@"/"];
    _app.setExecutiblePath( [executiblePath cStringUsingEncoding:NSUTF8StringEncoding] );
    
    [self setupGL];
}

/*
- (void)viewWillAppear:(BOOL)animated
{
}
*/

- (void)dealloc
{    
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        [self tearDownGL];
        
        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }

    // Dispose of any resources that can be recreated.
}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];
    
    int w = self.view.bounds.size.width*_scaleFactor;
    int h = self.view.bounds.size.height*_scaleFactor;
    
    // "create window"
    _app.createWindow("iosApp", 0, 0, w, h, false);
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
    
    _app.destroyWindow();
}

// Touch
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (UITouch *aTouch in touches)
	{
		FI::Event e;
		e.setType(FI::EVENT_MOUSE_LEFT_CLICK);
		float pos[2] = {
			(float)[aTouch locationInView:self.view].x*_scaleFactor,
			(float)[aTouch locationInView:self.view].y*_scaleFactor
		};
		e.setData(pos);
		_app.setEvent(e);
    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (UITouch *aTouch in touches)
	{
		FI::Event e;
		e.setType(FI::EVENT_MOUSE_LEFT_DRAG);
		float pos[2] = {
			(float)[aTouch locationInView:self.view].x*_scaleFactor,
			(float)[aTouch locationInView:self.view].y*_scaleFactor
		};
		e.setData(pos);
		_app.setEvent(e);
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (UITouch *aTouch in touches)
	{
		FI::Event e;
		e.setType(FI::EVENT_MOUSE_LEFT_RELEASE);
		float pos[2] = {
			(float)[aTouch locationInView:self.view].x*_scaleFactor,
			(float)[aTouch locationInView:self.view].y*_scaleFactor
		};
		e.setData(pos);
		_app.setEvent(e);
    }
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    // on iOS the currently framebuffer seems to have an ID thats
    // after all other textures. We need to look for it before rendering.
	if(!isFirstDraw)
	{
		//_app.setDefaultFramebufferFromCurrent();
		isFirstDraw = YES;
	}

    _app.drawScene();
}


@end
