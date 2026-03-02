#include "app_wgpu.h"

#include <string>

namespace {

WGPUStringView strView(const char* s)
{
	WGPUStringView v{};
	v.data = s;
	v.length = WGPU_STRLEN;
	return v;
}

WGPUShaderModule createShaderModule(WGPUDevice device, const char* wgsl)
{
	WGPUShaderSourceWGSL wgslDesc{};
	wgslDesc.chain.sType = WGPUSType_ShaderSourceWGSL;
	wgslDesc.code = strView(wgsl);

	WGPUShaderModuleDescriptor shaderDesc{};
	shaderDesc.nextInChain = &wgslDesc.chain;

	return wgpuDeviceCreateShaderModule(device, &shaderDesc);
}

std::string toStdString(WGPUStringView s)
{
	if (!s.data)
	{
		return "";
	}
	if (s.length == WGPU_STRLEN)
	{
		return std::string(s.data);
	}
	return std::string(s.data, s.length);
}

}

MyApp::MyApp(const int argc, const char *argv[])
	: WebApp(argc, argv)
{
	FI::LOG("MyApp(WebGPU) ctor");
	if (!cmdLineArg("-test", false).empty()) {
		FI::LOG("test specified!");
	}

	std::string v = cmdLineArg("-value", true);
	if (!v.empty()) {
		FI::LOG("value: ", v);
	}
}

MyApp::~MyApp()
{
}

void MyApp::onKeyPress(FI::Event e)
{
	eKeyCode kc = (eKeyCode)e.data_uint32()[0];
	switch(kc)
	{
		case KEY_ESC:
		case KEY_Q:
			m_bQuit = true;
			break;

		case KEY_W: FI::LOG("Key W pressed"); break;
		case KEY_A: FI::LOG("Key A pressed"); break;
		case KEY_S: FI::LOG("Key S pressed"); break;
		case KEY_D: FI::LOG("Key D pressed"); break;

		default:
			return;
	}
}

void MyApp::onKeyRelease(FI::Event e)
{
	eKeyCode kc = (eKeyCode)e.data_uint32()[0];
	switch(kc)
	{
		case KEY_ESC:
		case KEY_Q:
			m_bQuit = true;
			break;

		case KEY_W: FI::LOG("Key W released"); break;
		case KEY_A: FI::LOG("Key A released"); break;
		case KEY_S: FI::LOG("Key S released"); break;
		case KEY_D: FI::LOG("Key D released"); break;

		default:
			return;
	}
}

void MyApp::onMouseLeftClick(FI::Event e)
{
	float x = e.data_float()[0];
	float y = e.data_float()[1];
	FI::LOG("LMB click (", x, ",", y, ")");
}

void MyApp::onMouseLeftRelease(FI::Event e)
{
	float x = e.data_float()[0];
	float y = e.data_float()[1];
	FI::LOG("LMB release (", x, ",", y, ")");
}

void MyApp::onMouseRightClick(FI::Event e)
{
	float x = e.data_float()[0];
	float y = e.data_float()[1];
	FI::LOG("RMB click (", x, ",", y, ")");
}

void MyApp::onMouseRightRelease(FI::Event e)
{
	float x = e.data_float()[0];
	float y = e.data_float()[1];
	FI::LOG("RMB release (", x, ",", y, ")");
}

void MyApp::configureSurface()
{
	FI::LOG("configureSurface begin");
	if (!m_surface || !m_wgpuDevice)
	{
		m_surfaceConfigured = false;
		FI::LOG("configureSurface skipped: surface/device missing");
		return;
	}

	if ((m_width < 1 || m_height < 1) && mWindow)
	{
		SDL_GetWindowSize(mWindow, &m_width, &m_height);
	}
	if (m_width < 1) m_width = 640;
	if (m_height < 1) m_height = 480;

	WGPUSurfaceConfiguration config{};
	config.device = m_wgpuDevice;
	config.format = m_surfaceFormat;
	config.usage = WGPUTextureUsage_RenderAttachment;
	config.presentMode = WGPUPresentMode_Fifo;
	config.alphaMode = WGPUCompositeAlphaMode_Auto;
	config.width = static_cast<uint32_t>(m_width);
	config.height = static_cast<uint32_t>(m_height);
	FI::LOG("configureSurface request size=", m_width, "x", m_height, " format=", (int)m_surfaceFormat);

	wgpuSurfaceConfigure(m_surface, &config);
	m_surfaceConfigured = true;
	static bool once = false;
	if (!once)
	{
		FI::LOG("Configured WebGPU surface:", m_width, "x", m_height, " format=", (int)m_surfaceFormat);
		once = true;
	}
}

void MyApp::resize(int width, int height)
{
	FI::LOG("MyApp::resize ", width, "x", height);
	m_width = width;
	m_height = height;
	configureSurface();
}

void MyApp::gfxAPIInit()
{
	FI::LOG("initializing WebGPU scene...");
	startDeviceInit();
}

void MyApp::startDeviceInit()
{
	if (m_webgpuUnavailable || m_deviceInitRequested || m_deviceReady)
	{
		return;
	}

	if (!m_instance)
	{
		WGPUInstanceDescriptor instanceDesc{};
		m_instance = wgpuCreateInstance(&instanceDesc);
		if (!m_instance)
		{
			FI::LOG("Failed to create WebGPU instance");
			return;
		}
		FI::LOG("wgpuCreateInstance OK -> ", (void*)m_instance);
	}

	WGPURequestAdapterOptions options{};
	WGPURequestAdapterCallbackInfo cb{};
	cb.mode = WGPUCallbackMode_AllowSpontaneous;
	cb.callback = &MyApp::onRequestAdapter;
	cb.userdata1 = this;

	m_deviceInitRequested = true;
	wgpuInstanceRequestAdapter(m_instance, &options, cb);
	FI::LOG("Requested WebGPU adapter");
}

void MyApp::onRequestAdapter(WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, void* userdata1, void* userdata2)
{
	(void)userdata2;
	MyApp* self = static_cast<MyApp*>(userdata1);
	if (!self)
	{
		return;
	}

	if (status != WGPURequestAdapterStatus_Success || !adapter)
	{
		const std::string msg = toStdString(message);
		FI::LOG("RequestAdapter failed status=", (int)status, " msg=", msg);
		if (msg.find("navigator.gpu is not available") != std::string::npos)
		{
			self->m_webgpuUnavailable = true;
			FI::LOG("WebGPU unavailable in this browser/runtime. Use a WebGPU-enabled browser/context.");
		}
		self->m_deviceInitRequested = false;
		return;
	}

	self->m_adapter = adapter;
	FI::LOG("WebGPU adapter acquired");

	WGPUDeviceDescriptor deviceDesc{};
	WGPURequestDeviceCallbackInfo cb{};
	cb.mode = WGPUCallbackMode_AllowSpontaneous;
	cb.callback = &MyApp::onRequestDevice;
	cb.userdata1 = self;
	wgpuAdapterRequestDevice(adapter, &deviceDesc, cb);
	FI::LOG("Requested WebGPU device");
}

void MyApp::onRequestDevice(WGPURequestDeviceStatus status, WGPUDevice device, WGPUStringView message, void* userdata1, void* userdata2)
{
	(void)userdata2;
	MyApp* self = static_cast<MyApp*>(userdata1);
	if (!self)
	{
		return;
	}

	if (status != WGPURequestDeviceStatus_Success || !device)
	{
		FI::LOG("RequestDevice failed status=", (int)status, " msg=", toStdString(message));
		self->m_deviceInitRequested = false;
		return;
	}

	self->m_wgpuDevice = device;
	self->m_queue = wgpuDeviceGetQueue(device);
	self->m_deviceReady = true;
	FI::LOG("WebGPU device acquired -> ", (void*)device, " queue=", (void*)self->m_queue);

	self->finishInitWithDevice();
}

void MyApp::finishInitWithDevice()
{
	if (!m_wgpuDevice || !m_instance)
	{
		FI::LOG("finishInitWithDevice skipped: missing device/instance");
		return;
	}

	WGPUEmscriptenSurfaceSourceCanvasHTMLSelector canvasDesc{};
	canvasDesc.chain.sType = WGPUSType_EmscriptenSurfaceSourceCanvasHTMLSelector;
	canvasDesc.selector = strView("#canvas");

	WGPUSurfaceDescriptor surfaceDesc{};
	surfaceDesc.nextInChain = &canvasDesc.chain;

	m_surface = wgpuInstanceCreateSurface(m_instance, &surfaceDesc);
	if (!m_surface)
	{
		FI::LOG("Failed to create WebGPU surface");
		return;
	}
	FI::LOG("wgpuInstanceCreateSurface OK -> ", (void*)m_surface);

	configureSurface();
	FI::LOG("configureSurface done, m_surfaceConfigured=", m_surfaceConfigured);

	const char* shaderSrc =
		R"(
@vertex
fn vs_main(@builtin(vertex_index) vi : u32) -> @builtin(position) vec4<f32> {
  var pos = array<vec2<f32>, 3>(
    vec2<f32>(0.0, 0.5),
    vec2<f32>(-0.5, -0.5),
    vec2<f32>(0.5, -0.5));
  return vec4<f32>(pos[vi], 0.0, 1.0);
}
@fragment
fn fs_main() -> @location(0) vec4<f32> {
  return vec4<f32>(1.0, 0.0, 0.0, 1.0);
}
)";

	WGPUShaderModule shaderModule = createShaderModule(m_wgpuDevice, shaderSrc);
	if (!shaderModule)
	{
		FI::LOG("Failed to create WebGPU shader module");
		return;
	}
	FI::LOG("Shader module created");

	WGPUColorTargetState colorTarget{};
	colorTarget.format = m_surfaceFormat;
	colorTarget.writeMask = WGPUColorWriteMask_All;

	WGPUFragmentState fragment{};
	fragment.module = shaderModule;
	fragment.entryPoint = strView("fs_main");
	fragment.targetCount = 1;
	fragment.targets = &colorTarget;

	WGPUVertexState vertex{};
	vertex.module = shaderModule;
	vertex.entryPoint = strView("vs_main");
	vertex.bufferCount = 0;
	vertex.buffers = nullptr;

	WGPUPipelineLayoutDescriptor pipelineLayoutDesc{};
	m_pipelineLayout = wgpuDeviceCreatePipelineLayout(m_wgpuDevice, &pipelineLayoutDesc);
	if (!m_pipelineLayout)
	{
		FI::LOG("Failed to create WebGPU pipeline layout");
		wgpuShaderModuleRelease(shaderModule);
		return;
	}
	FI::LOG("Pipeline layout created");

	WGPURenderPipelineDescriptor pipelineDesc{};
	pipelineDesc.layout = m_pipelineLayout;
	pipelineDesc.vertex = vertex;
	pipelineDesc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
	pipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
	pipelineDesc.primitive.frontFace = WGPUFrontFace_CCW;
	pipelineDesc.primitive.cullMode = WGPUCullMode_None;
	pipelineDesc.multisample.count = 1;
	pipelineDesc.multisample.mask = ~0u;
	pipelineDesc.multisample.alphaToCoverageEnabled = false;
	pipelineDesc.fragment = &fragment;

	m_pipeline = wgpuDeviceCreateRenderPipeline(m_wgpuDevice, &pipelineDesc);
	if (!m_pipeline)
	{
		FI::LOG("Failed to create WebGPU render pipeline");
	}
	else
	{
		FI::LOG("Render pipeline created");
	}

	wgpuShaderModuleRelease(shaderModule);
	FI::LOG("WebGPU init complete");
}

void MyApp::gfxAPIDraw()
{
	static bool firstDrawLogged = false;
	if (!firstDrawLogged)
	{
		FI::LOG("gfxAPIDraw entered");
		firstDrawLogged = true;
	}

	if (!m_surfaceConfigured)
	{
		static bool noSurfaceLogged = false;
		if (!noSurfaceLogged)
		{
			FI::LOG("gfxAPIDraw skipped: surface not configured");
			noSurfaceLogged = true;
		}
		if (!m_webgpuUnavailable && !m_deviceInitRequested)
		{
			startDeviceInit();
		}
		return;
	}

	WGPUSurfaceTexture currentTexture{};
	wgpuSurfaceGetCurrentTexture(m_surface, &currentTexture);

	if (currentTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal &&
		currentTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal)
	{
		static int statusLogCount = 0;
		if (statusLogCount < 10)
		{
			FI::LOG("wgpuSurfaceGetCurrentTexture status=", (int)currentTexture.status);
			++statusLogCount;
		}
		configureSurface();
		return;
	}
	if (!currentTexture.texture)
	{
		static int nullTexLogCount = 0;
		if (nullTexLogCount < 10)
		{
			FI::LOG("wgpuSurfaceGetCurrentTexture returned null texture");
			++nullTexLogCount;
		}
		return;
	}

	WGPUTextureView backbufferView = wgpuTextureCreateView(currentTexture.texture, nullptr);
	if (!backbufferView)
	{
		FI::LOG("wgpuTextureCreateView failed");
		wgpuTextureRelease(currentTexture.texture);
		return;
	}

	WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(m_wgpuDevice, nullptr);
	if (!encoder)
	{
		FI::LOG("wgpuDeviceCreateCommandEncoder failed");
		wgpuTextureViewRelease(backbufferView);
		wgpuTextureRelease(currentTexture.texture);
		return;
	}

	WGPURenderPassColorAttachment colorAttachment{};
	colorAttachment.view = backbufferView;
	colorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
	colorAttachment.resolveTarget = nullptr;
	colorAttachment.loadOp = WGPULoadOp_Clear;
	colorAttachment.storeOp = WGPUStoreOp_Store;
	colorAttachment.clearValue = {0.0, 0.2, 0.1, 1.0};

	WGPURenderPassDescriptor passDesc{};
	passDesc.colorAttachmentCount = 1;
	passDesc.colorAttachments = &colorAttachment;

	WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(encoder, &passDesc);
	if (m_pipeline)
	{
		wgpuRenderPassEncoderSetPipeline(pass, m_pipeline);
		wgpuRenderPassEncoderDraw(pass, 3, 1, 0, 0);
	}
	else
	{
		static bool pipelineMissingLogged = false;
		if (!pipelineMissingLogged)
		{
			FI::LOG("WebGPU pipeline missing: clear-only frame");
			pipelineMissingLogged = true;
		}
	}
	wgpuRenderPassEncoderEnd(pass);

	WGPUCommandBuffer cmd = wgpuCommandEncoderFinish(encoder, nullptr);
	if (!cmd)
	{
		FI::LOG("wgpuCommandEncoderFinish failed");
	}
	wgpuQueueSubmit(m_queue, 1, &cmd);
#if !defined(WEB)
	WGPUStatus presentStatus = wgpuSurfacePresent(m_surface);
	if (presentStatus != WGPUStatus_Success)
	{
		static bool presentLogged = false;
		if (!presentLogged)
		{
			FI::LOG("wgpuSurfacePresent failed, status=", (int)presentStatus);
			presentLogged = true;
		}
	}
#endif

	wgpuCommandBufferRelease(cmd);
	wgpuRenderPassEncoderRelease(pass);
	wgpuCommandEncoderRelease(encoder);
	wgpuTextureViewRelease(backbufferView);
	wgpuTextureRelease(currentTexture.texture);
}

void MyApp::gfxAPIDeinit()
{
	FI::LOG("de-initializing WebGPU scene");

	if (m_pipeline)
	{
		wgpuRenderPipelineRelease(m_pipeline);
		m_pipeline = nullptr;
	}
	if (m_pipelineLayout)
	{
		wgpuPipelineLayoutRelease(m_pipelineLayout);
		m_pipelineLayout = nullptr;
	}
	if (m_surface)
	{
		wgpuSurfaceRelease(m_surface);
		m_surface = nullptr;
	}
	if (m_instance)
	{
		wgpuInstanceRelease(m_instance);
		m_instance = nullptr;
	}
	if (m_adapter)
	{
		wgpuAdapterRelease(m_adapter);
		m_adapter = nullptr;
	}
	if (m_wgpuDevice)
	{
		wgpuDeviceRelease(m_wgpuDevice);
		m_wgpuDevice = nullptr;
	}
	m_queue = nullptr;
	m_surfaceConfigured = false;
	m_deviceReady = false;
	m_deviceInitRequested = false;
	m_webgpuUnavailable = false;
}
