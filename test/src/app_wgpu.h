#ifndef _APP_WGPU_H
#define _APP_WGPU_H

#if !defined(FI_GFX_WEBGPU)
#error "app_wgpu.h requires FI_GFX_WEBGPU"
#endif

#include "fi/app/webApp.h"

class MyApp : public WebApp
{
public:
	MyApp(const int argc = 0, const char *argv[] = nullptr);
	virtual ~MyApp();

	// Application overrides
	void resize(int width, int height) override;
	void gfxAPIInit() override;
	void gfxAPIDraw() override;
	void gfxAPIDeinit() override;

	// EventListener overrides
	void onKeyPress(FI::Event e) override;
	void onKeyRelease(FI::Event e) override;
	void onMouseLeftClick(FI::Event e) override;
	void onMouseLeftRelease(FI::Event e) override;
	void onMouseRightClick(FI::Event e) override;
	void onMouseRightRelease(FI::Event e) override;

private:
	static void onRequestAdapter(WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, void* userdata1, void* userdata2);
	static void onRequestDevice(WGPURequestDeviceStatus status, WGPUDevice device, WGPUStringView message, void* userdata1, void* userdata2);

	void startDeviceInit();
	void finishInitWithDevice();
	void configureSurface();

	WGPUInstance m_instance = nullptr;
	WGPUAdapter m_adapter = nullptr;
	WGPUSurface m_surface = nullptr;
	WGPUQueue m_queue = nullptr;
	WGPUPipelineLayout m_pipelineLayout = nullptr;
	WGPURenderPipeline m_pipeline = nullptr;
	WGPUTextureFormat m_surfaceFormat = WGPUTextureFormat_BGRA8Unorm;
	bool m_surfaceConfigured = false;
	bool m_deviceInitRequested = false;
	bool m_deviceReady = false;
	bool m_webgpuUnavailable = false;
};

#endif //_APP_WGPU_H
