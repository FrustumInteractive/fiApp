/*
* d3d9App.cpp
*
*	Created on: Aug 10, 2015
*		Author: Roger Dass
*		Copyright Frustum Interactive Inc. - All rights reserved.
*/

#include "fi/app/D3D9App.h"
#include <stdio.h>

D3D9App::D3D9App(const int argc, const char *argv[]) :
	Win32App(argc, argv),
	m_pD3D(0),
	m_pD3DDevice(0)
{
}

D3D9App::~D3D9App()
{
}

void D3D9App::gfxAPIInit()
{
	printf("Initializing Direct3D 9 Subsystem\n");
	// Check For The Correct DirectX 3D version
	m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	if ( m_pD3D == NULL )
	{
		destroyWindow();		// Reset The Display
		MessageBox(NULL, "Can't find D3D SDK Version 9.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return;		// Return FALSE
	}

	// Tell the window how we want things to be..
	D3DPRESENT_PARAMETERS d3dpp =
	{
		(UINT)m_width,			// Back Buffer Width
		(UINT)m_height,			// Back Buffer Height
		D3DFMT_X8R8G8B8,		// Back Buffer Format (Color Depth)
		1,						// Back Buffer Count (Double Buffer)
		D3DMULTISAMPLE_NONE,	// No Multi Sample Type
		0,						// No Multi Sample Quality
		D3DSWAPEFFECT_DISCARD,	// Swap Effect (Fast)
		m_hWnd,					// The Window Handle
		!m_bFullscreen,			// Windowed or Fullscreen
		TRUE,					// Enable Auto Depth Stencil  
		D3DFMT_D16,				// 16Bit Z-Buffer (Depth Buffer)
		0,						// No Flags
		D3DPRESENT_RATE_DEFAULT,	// Default Refresh Rate
		D3DPRESENT_INTERVAL_DEFAULT	// Presentation Interval (vertical sync)
	};

	// Check The Wanted Surface Format
	if ( FAILED( m_pD3D->CheckDeviceFormat(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		d3dpp.BackBufferFormat,
		D3DUSAGE_DEPTHSTENCIL,
		D3DRTYPE_SURFACE,
		d3dpp.AutoDepthStencilFormat ) ) )
	{
		destroyWindow();		// Reset The Display
		MessageBox(NULL, "Can't Find Surface Format.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return;		// Return FALSE
	}

	// Create The DirectX 3D Device
	if(FAILED( m_pD3D->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		m_hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &m_pD3DDevice ) ) )
	{
		destroyWindow();	// Reset The Display
		MessageBox(NULL, "Can't Create DirectX 3D Device.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return;	// Return FALSE
	}

	ShowWindow( m_hWnd, SW_SHOW );	// Show The Window
	SetForegroundWindow( m_hWnd );	// Slightly Higher Priority
	SetFocus( m_hWnd );				// Sets Focus To The Window
	

	// Initialize Our Newly Created D3D Window
	destroyWindow();		// Reset The Display
	MessageBox(NULL, "Initialization Failed.",	"ERROR", MB_OK|MB_ICONEXCLAMATION);
	return;			// Return FALSE

	resize( m_width, m_height );		// Set Up Our Perspective D3D Screen

	return;
}

void D3D9App::gfxAPIDraw()
{
	// Clear Screen And Depth Buffer
	m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,0.0f), 1.0f, 0 );

	m_pD3DDevice->BeginScene();

	// Place your code here!

	m_pD3DDevice->EndScene();

	m_pD3DDevice->Present( NULL, NULL, NULL, NULL );	// Display Result

	SwapBuffers(m_hDC);

	return;
}

void D3D9App::gfxAPIDeinit()
{
	printf("Shutting Down Direct3D 9...\n");
	deinitScene();		//clean up D3D initializations

	if (m_pD3DDevice!=NULL)	m_pD3DDevice->Release();	// Release D3D Device

	if (m_pD3D!=NULL) m_pD3D->Release();				// Release D3D Interface

	return;
}
