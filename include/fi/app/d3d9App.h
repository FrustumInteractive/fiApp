/*
* d3d9App.h
*
*  Created on: Aug 10, 2015
*      Author: Roger Dass
*		Copyright Frustum Interactive Inc. - All rights reserved.
*/

#include <d3d9.h>
#include "Win32App.h"

class D3D9App : public Win32App
{
public:
	
	D3D9App(const int argc=0, const char *argv[]=0);
	virtual ~D3D9App();

	virtual void	initScene() {}
	virtual void	deinitScene() {}
	virtual void	drawScene() {}

	void		gfxAPIInit();
	void		gfxAPIDraw();
	void		gfxAPIDeinit();

protected:

	LPDIRECT3D9			m_pD3D;			// DirectX 3D Version 9
	LPDIRECT3DDEVICE9	m_pD3DDevice;	// DirectX 3D Rendering Device
};
