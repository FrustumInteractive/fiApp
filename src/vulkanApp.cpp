// vulkanApp.cpp
/*
 * VulkanApp.cpp
 *
 *  Created on: Feb 19, 2026
 *      Author: Roger Dass
 *      Copyright Frustum Interactive Inc. - All rights reserved.
 */

#include "fi/app/vulkanApp.h"
#include "fi/debug/trace.h"

#include <stdexcept>
#include <vector>
#include <cstring>
#include <algorithm>

#if defined(OSX)
	#if defined(FI_GFX_METAL)
		#include "fi/app/cocoaWrapper.h"
		#include <vulkan/vulkan_metal.h>
	#endif
#endif

static inline void vkCheck(VkResult r, const char *msg)
{
	if (r != VK_SUCCESS)
		throw std::runtime_error(msg);
}

VulkanApp::VulkanApp(const int argc, const char *argv[])
#if defined(WIN32)
	: Win32App(argc, argv)
#elif defined(WEB)
	: WebApp(argc, argv)
#elif defined(LINUX)
	: X11App(argc, argv)
#elif defined(OSX)
	: OSXApp(argc, argv)
#else
	: MobileApp(argc, argv)
#endif
{
}

VulkanApp::~VulkanApp()
{
}

void VulkanApp::setPresentEnabled(bool enabled)
{
	m_presentEnabled = enabled;
}

bool VulkanApp::isPresentEnabled() const
{
	return m_presentEnabled;
}

void VulkanApp::resize(int width, int height)
{
	(void)width;
	(void)height;
	// Don't recreate immediately here; just request it.
	// The swapchain recreate will happen on the next beginFrame or present result.
	requestSwapchainRecreate();
}

void VulkanApp::gfxAPIInit()
{
	try
	{
		createInstance();

		if (m_presentEnabled)
			createSurface();

		pickPhysicalDevice();
		createDeviceAndQueues();

		if (m_presentEnabled)
			createSwapchain();

		createCommandPools();
		createFrameSync();

		initScene();
	}
	catch (const std::exception &e)
	{
		FI::LOG("VulkanApp init failed: ", e.what());
		// Best-effort cleanup
		gfxAPIDeinit();
	}
}

void VulkanApp::gfxAPIDraw()
{
	if (!m_device)
		return;

	if (!beginFrame())
		return;

	// Let app do whatever: multiple command buffers, queues, etc.
	drawScene();

	if (m_presentEnabled)
	{
		// If frameIndex did not advance, app probably didn't call present().
		// This will eventually deadlock on vkAcquireNextImageKHR.
		if (m_frameIndex == m_frame.frameIndex)
		{
			FI::LOG("WARNING: onVkFrame() did not present() or advance frame index; swapchain may stall.");
		}
	}
	else
	{
		m_frameIndex = (m_frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
	}
}

void VulkanApp::gfxAPIDeinit()
{
	if (!m_device && !m_instance)
		return;

	try
	{
		if (m_device)
			vkDeviceWaitIdle(m_device);
	}
	catch (...)
	{
	}

	// App shutdown hook
	try
	{
		deinitScene();
	}
	catch (...)
	{
	}

	// Destroy sync/pools/swapchain/core
	destroyFrameSync();
	destroyCommandPools();
	destroySwapchain();

	if (m_device)
	{
		vkDestroyDevice(m_device, nullptr);
		m_device = VK_NULL_HANDLE;
	}

	if (m_surface)
	{
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
		m_surface = VK_NULL_HANDLE;
	}

	if (m_instance)
	{
		vkDestroyInstance(m_instance, nullptr);
		m_instance = VK_NULL_HANDLE;
	}

	m_phys = VK_NULL_HANDLE;
}

void VulkanApp::requestSwapchainRecreate()
{
	m_swapchainRecreateRequested = true;
}

// ----------------- Instance / Device -----------------

void VulkanApp::createInstance()
{
	std::vector<const char *> instExt;
	instExt.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

#if defined(OSX)
	// On macOS/MoltenVK you typically need metal surface + portability enumeration.
	instExt.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
	instExt.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

	VkApplicationInfo appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
	appInfo.pApplicationName = "fiEngine VulkanApp";
	appInfo.apiVersion = VK_API_VERSION_1_2;

	VkInstanceCreateInfo ici{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
	ici.pApplicationInfo = &appInfo;
	ici.enabledExtensionCount = (uint32_t)instExt.size();
	ici.ppEnabledExtensionNames = instExt.data();

#if defined(OSX)
	ici.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

	vkCheck(vkCreateInstance(&ici, nullptr, &m_instance), "vkCreateInstance failed");
}

void VulkanApp::createSurface()
{
#if defined(OSX)
	#if !defined(FI_GFX_METAL)
		throw std::runtime_error("OSX Vulkan surface requires FI_GFX_METAL (CAMetalLayer).");
	#else
		void *layerVoid = CWGetMetalLayer();
		if (!layerVoid)
			throw std::runtime_error("CWGetMetalLayer returned null");

		VkMetalSurfaceCreateInfoEXT msci{VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT};
		msci.pLayer = (CAMetalLayer *)layerVoid;
		vkCheck(vkCreateMetalSurfaceEXT(m_instance, &msci, nullptr, &m_surface),
				"vkCreateMetalSurfaceEXT failed");
	#endif
#else
	// TODO: Win32/X11/Android surface creation based on your native window handles.
	throw std::runtime_error("createSurface not implemented for this platform yet.");
#endif
}

uint32_t VulkanApp::findQueueFamily(VkQueueFlags required, VkQueueFlags excluded) const
{
	uint32_t qCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(m_phys, &qCount, nullptr);
	std::vector<VkQueueFamilyProperties> qprops(qCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_phys, &qCount, qprops.data());

	for (uint32_t i = 0; i < qCount; ++i)
	{
		const VkQueueFlags f = qprops[i].queueFlags;
		if ((f & required) == required && (excluded == 0 || (f & excluded) == 0))
			return i;
	}
	return UINT32_MAX;
}

uint32_t VulkanApp::findPresentFamily() const
{
	if (!m_presentEnabled || !m_surface)
		return UINT32_MAX;

	uint32_t qCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(m_phys, &qCount, nullptr);
	for (uint32_t i = 0; i < qCount; ++i)
	{
		VkBool32 supported = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(m_phys, i, m_surface, &supported);
		if (supported)
			return i;
	}
	return UINT32_MAX;
}

void VulkanApp::pickPhysicalDevice()
{
	uint32_t devCount = 0;
	vkCheck(vkEnumeratePhysicalDevices(m_instance, &devCount, nullptr),
			"vkEnumeratePhysicalDevices count failed");
	if (devCount == 0)
		throw std::runtime_error("No Vulkan physical devices found");

	std::vector<VkPhysicalDevice> devs(devCount);
	vkCheck(vkEnumeratePhysicalDevices(m_instance, &devCount, devs.data()),
			"vkEnumeratePhysicalDevices list failed");

	// Minimal selection: device that can do graphics (and present if enabled).
	for (auto pd : devs)
	{
		m_phys = pd;

		uint32_t g = findQueueFamily(VK_QUEUE_GRAPHICS_BIT);
		if (g == UINT32_MAX)
			continue;

		if (m_presentEnabled)
		{
			uint32_t p = findPresentFamily();
			if (p == UINT32_MAX)
				continue;
		}

		// OK
		return;
	}

	m_phys = VK_NULL_HANDLE;
	throw std::runtime_error("No suitable physical device found");
}

void VulkanApp::createDeviceAndQueues()
{
	// Choose queue families.
	m_queues.graphicsFamily = findQueueFamily(VK_QUEUE_GRAPHICS_BIT);
	if (m_queues.graphicsFamily == UINT32_MAX)
		throw std::runtime_error("No graphics queue family");

	m_queues.presentFamily = m_presentEnabled ? findPresentFamily() : UINT32_MAX;

	// Optional: dedicated compute/transfer if available, otherwise fall back.
	// Prefer compute without graphics bit for async compute (when available).
	m_queues.computeFamily = findQueueFamily(VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT);
	if (m_queues.computeFamily == UINT32_MAX)
		m_queues.computeFamily = m_queues.graphicsFamily;

	// Prefer transfer-only queue (when available).
	m_queues.transferFamily = findQueueFamily(VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
	if (m_queues.transferFamily == UINT32_MAX)
		m_queues.transferFamily = m_queues.graphicsFamily;

	float prio = 1.0f;
	std::vector<VkDeviceQueueCreateInfo> qcis;

	auto addFamily = [&](uint32_t fam)
	{
		for (auto &existing : qcis)
			if (existing.queueFamilyIndex == fam)
				return;
		VkDeviceQueueCreateInfo qci{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
		qci.queueFamilyIndex = fam;
		qci.queueCount = 1;
		qci.pQueuePriorities = &prio;
		qcis.push_back(qci);
	};

	addFamily(m_queues.graphicsFamily);
	if (m_queues.presentFamily != UINT32_MAX)
		addFamily(m_queues.presentFamily);
	addFamily(m_queues.computeFamily);
	addFamily(m_queues.transferFamily);

	// Device extensions
	std::vector<const char *> devExt;
	if (m_presentEnabled)
		devExt.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

#if defined(OSX)
	// MoltenVK often exposes this; enabling it is harmless if present.
	// (If you want strictness, enumerate device extensions and check first.)
	devExt.push_back("VK_KHR_portability_subset");
#endif

	VkDeviceCreateInfo dci{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
	dci.queueCreateInfoCount = (uint32_t)qcis.size();
	dci.pQueueCreateInfos = qcis.data();
	dci.enabledExtensionCount = (uint32_t)devExt.size();
	dci.ppEnabledExtensionNames = devExt.data();

	vkCheck(vkCreateDevice(m_phys, &dci, nullptr, &m_device), "vkCreateDevice failed");

	vkGetDeviceQueue(m_device, m_queues.graphicsFamily, 0, &m_queues.graphics);
	if (m_queues.presentFamily != UINT32_MAX)
		vkGetDeviceQueue(m_device, m_queues.presentFamily, 0, &m_queues.present);

	vkGetDeviceQueue(m_device, m_queues.computeFamily, 0, &m_queues.compute);
	vkGetDeviceQueue(m_device, m_queues.transferFamily, 0, &m_queues.transfer);

	// If some families alias, queues may be the same handle — that's fine.
}

// ----------------- Swapchain -----------------

void VulkanApp::createSwapchain()
{
	if (!m_presentEnabled)
		return;
	if (!m_surface)
		throw std::runtime_error("createSwapchain: surface is null");

	VkSurfaceCapabilitiesKHR caps{};
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_phys, m_surface, &caps);

	uint32_t fmtCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_phys, m_surface, &fmtCount, nullptr);
	std::vector<VkSurfaceFormatKHR> formats(fmtCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_phys, m_surface, &fmtCount, formats.data());
	if (formats.empty())
		throw std::runtime_error("No surface formats");

	VkSurfaceFormatKHR chosen = formats[0];
	for (auto &f : formats)
	{
		if (f.format == VK_FORMAT_B8G8R8A8_SRGB && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			chosen = f;
	}
	m_swapchainFormat = chosen.format;

	VkExtent2D extent = caps.currentExtent;
	if (extent.width == 0xFFFFFFFF)
	{
		// Your CWGetWindowSize on Metal path should return drawable pixels.
		int w = 0, h = 0;
#if defined(OSX)
		CWGetWindowSize(w, h);
#else
		// TODO: other platforms: your app framework should return drawable size
#endif
		extent.width = (uint32_t)std::clamp(w, (int)caps.minImageExtent.width, (int)caps.maxImageExtent.width);
		extent.height = (uint32_t)std::clamp(h, (int)caps.minImageExtent.height, (int)caps.maxImageExtent.height);
	}
	m_swapchainExtent = extent;

	uint32_t pmCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_phys, m_surface, &pmCount, nullptr);
	std::vector<VkPresentModeKHR> pms(pmCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_phys, m_surface, &pmCount, pms.data());

	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
	for (auto m : pms)
		if (m == VK_PRESENT_MODE_MAILBOX_KHR)
			presentMode = m;

	uint32_t imageCount = caps.minImageCount + 1;
	if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount)
		imageCount = caps.maxImageCount;

	VkSwapchainCreateInfoKHR sci{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
	sci.surface = m_surface;
	sci.minImageCount = imageCount;
	sci.imageFormat = chosen.format;
	sci.imageColorSpace = chosen.colorSpace;
	sci.imageExtent = extent;
	sci.imageArrayLayers = 1;
	sci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t qfi[2] = {m_queues.graphicsFamily, m_queues.presentFamily};
	if (m_queues.presentFamily != UINT32_MAX && m_queues.presentFamily != m_queues.graphicsFamily)
	{
		sci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		sci.queueFamilyIndexCount = 2;
		sci.pQueueFamilyIndices = qfi;
	}
	else
	{
		sci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	sci.preTransform = caps.currentTransform;
	sci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	sci.presentMode = presentMode;
	sci.clipped = VK_TRUE;

	vkCheck(vkCreateSwapchainKHR(m_device, &sci, nullptr, &m_swapchain), "vkCreateSwapchainKHR failed");

	uint32_t scCount = 0;
	vkGetSwapchainImagesKHR(m_device, m_swapchain, &scCount, nullptr);
	m_swapchainImages.resize(scCount);
	vkGetSwapchainImagesKHR(m_device, m_swapchain, &scCount, m_swapchainImages.data());

	// Image views
	m_swapchainImageViews.resize(scCount);
	for (uint32_t i = 0; i < scCount; ++i)
	{
		VkImageViewCreateInfo iv{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
		iv.image = m_swapchainImages[i];
		iv.viewType = VK_IMAGE_VIEW_TYPE_2D;
		iv.format = m_swapchainFormat;
		iv.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		iv.subresourceRange.levelCount = 1;
		iv.subresourceRange.layerCount = 1;
		vkCheck(vkCreateImageView(m_device, &iv, nullptr, &m_swapchainImageViews[i]), "vkCreateImageView failed");
	}

	imagesInFlight.assign(m_swapchainImages.size(), VK_NULL_HANDLE);
	m_swapchainRecreateRequested = false;
}

void VulkanApp::destroySwapchain()
{
	if (!m_presentEnabled)
		return;

	for (auto iv : m_swapchainImageViews)
		if (iv)
			vkDestroyImageView(m_device, iv, nullptr);
	m_swapchainImageViews.clear();

	if (m_swapchain)
	{
		vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
		m_swapchain = VK_NULL_HANDLE;
	}

	m_swapchainImages.clear();
	imagesInFlight.clear();
}

void VulkanApp::recreateSwapchainIfNeeded()
{
	if (!m_presentEnabled)
		return;
	if (!m_swapchainRecreateRequested)
		return;

	vkDeviceWaitIdle(m_device);
	destroySwapchain();
	createSwapchain();
}

// ----------------- Command Pools / Frame Sync -----------------

void VulkanApp::createCommandPools()
{
	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		// Graphics pool
		VkCommandPoolCreateInfo pci{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
		pci.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		pci.queueFamilyIndex = m_queues.graphicsFamily;
		vkCheck(vkCreateCommandPool(m_device, &pci, nullptr, &m_pools[i].graphics), "vkCreateCommandPool graphics failed");

		pci.queueFamilyIndex = m_queues.computeFamily;
		vkCheck(vkCreateCommandPool(m_device, &pci, nullptr, &m_pools[i].compute), "vkCreateCommandPool compute failed");

		pci.queueFamilyIndex = m_queues.transferFamily;
		vkCheck(vkCreateCommandPool(m_device, &pci, nullptr, &m_pools[i].transfer), "vkCreateCommandPool transfer failed");
	}
}

void VulkanApp::destroyCommandPools()
{
	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		if (m_pools[i].graphics)
			vkDestroyCommandPool(m_device, m_pools[i].graphics, nullptr);
		if (m_pools[i].compute)
			vkDestroyCommandPool(m_device, m_pools[i].compute, nullptr);
		if (m_pools[i].transfer)
			vkDestroyCommandPool(m_device, m_pools[i].transfer, nullptr);
		m_pools[i] = {};
	}
}

void VulkanApp::createFrameSync()
{
	VkFenceCreateInfo fci{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
	fci.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VkSemaphoreCreateInfo sci{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		vkCheck(vkCreateFence(m_device, &fci, nullptr, &m_inFlightFences[i]), "vkCreateFence failed");

		if (m_presentEnabled)
			vkCheck(vkCreateSemaphore(m_device, &sci, nullptr, &m_imageAvailable[i]), "vkCreateSemaphore imageAvailable failed");
		else
			m_imageAvailable[i] = VK_NULL_HANDLE;
	}
}

void VulkanApp::destroyFrameSync()
{
	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		if (m_imageAvailable[i])
			vkDestroySemaphore(m_device, m_imageAvailable[i], nullptr);
		if (m_inFlightFences[i])
			vkDestroyFence(m_device, m_inFlightFences[i], nullptr);
		m_imageAvailable[i] = VK_NULL_HANDLE;
		m_inFlightFences[i] = VK_NULL_HANDLE;
	}
}

// ----------------- Frame begin/end -----------------

void VulkanApp::resetPerFramePools(uint32_t frameIndex)
{
	// Reset per-frame command pools at frame start (cheap and keeps life simple).
	vkResetCommandPool(m_device, m_pools[frameIndex].graphics, 0);
	vkResetCommandPool(m_device, m_pools[frameIndex].compute, 0);
	vkResetCommandPool(m_device, m_pools[frameIndex].transfer, 0);
}

bool VulkanApp::beginFrame()
{
	recreateSwapchainIfNeeded();

	const uint32_t f = m_frameIndex;

	// CPU/GPU sync for per-frame resources
	vkWaitForFences(m_device, 1, &m_inFlightFences[f], VK_TRUE, UINT64_MAX);
	vkResetFences(m_device, 1, &m_inFlightFences[f]);

	resetPerFramePools(f);

	m_frame = {};
	m_frame.frameIndex = f;
	m_frame.presenting = m_presentEnabled;
	m_frame.device = m_device;
	m_frame.queues = m_queues;
	m_frame.frameFence = m_inFlightFences[f];
	m_frame.imageAvailable = m_presentEnabled ? m_imageAvailable[f] : VK_NULL_HANDLE;

	if (!m_presentEnabled)
		return true;

	uint32_t imageIndex = 0;
	VkResult ar = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, m_imageAvailable[f], VK_NULL_HANDLE, &imageIndex);

	if (ar == VK_ERROR_OUT_OF_DATE_KHR)
	{
		requestSwapchainRecreate();
		return false;
	}
	if (ar != VK_SUCCESS && ar != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("vkAcquireNextImageKHR failed");
	}

	// If this swapchain image is already in flight, wait for its fence
	if (imagesInFlight.size() == m_swapchainImages.size() && imagesInFlight[imageIndex] != VK_NULL_HANDLE)
	{
		vkWaitForFences(m_device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}
	if (imagesInFlight.size() == m_swapchainImages.size())
		imagesInFlight[imageIndex] = m_inFlightFences[f];

	m_frame.imageIndex = imageIndex;
	return true;
}

void VulkanApp::endFrame(const std::vector<VkSemaphore> &presentWait)
{
	if (!m_presentEnabled)
		return;
	present(presentWait);
}

void VulkanApp::present(const std::vector<VkSemaphore> &waitSemaphores)
{
	if (!m_presentEnabled)
		return;

	VkPresentInfoKHR pi{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
	pi.waitSemaphoreCount = (uint32_t)waitSemaphores.size();
	pi.pWaitSemaphores = waitSemaphores.empty() ? nullptr : waitSemaphores.data();
	pi.swapchainCount = 1;
	pi.pSwapchains = &m_swapchain;
	pi.pImageIndices = &m_frame.imageIndex;

	VkQueue pq = (m_queues.present != VK_NULL_HANDLE) ? m_queues.present : m_queues.graphics;

	VkResult pr = vkQueuePresentKHR(pq, &pi);
	if (pr == VK_ERROR_OUT_OF_DATE_KHR || pr == VK_SUBOPTIMAL_KHR)
	{
		requestSwapchainRecreate();
	}

	// IMPORTANT: frame progress happens here for presenting apps.
	m_frameIndex = (m_frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

// ----------------- Command helpers -----------------

static VkCommandBuffer beginOneTime(VkDevice dev, VkCommandPool pool)
{
	VkCommandBufferAllocateInfo ai{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
	ai.commandPool = pool;
	ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	ai.commandBufferCount = 1;

	VkCommandBuffer cmd = VK_NULL_HANDLE;
	vkCheck(vkAllocateCommandBuffers(dev, &ai, &cmd), "vkAllocateCommandBuffers failed");

	VkCommandBufferBeginInfo bi{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
	bi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkCheck(vkBeginCommandBuffer(cmd, &bi), "vkBeginCommandBuffer failed");
	return cmd;
}

static void endOneTime(VkDevice dev, VkCommandPool pool, VkCommandBuffer cmd)
{
	vkCheck(vkEndCommandBuffer(cmd), "vkEndCommandBuffer failed");
	// We do NOT free here; we reset pool each frame. If you want to free, call vkFreeCommandBuffers.
	(void)dev;
	(void)pool;
	(void)cmd;
}

VkCommandBuffer VulkanApp::beginCommandsGraphics()
{
	return beginOneTime(m_device, m_pools[m_frameIndex].graphics);
}

void VulkanApp::endCommandsGraphics(VkCommandBuffer cmd)
{
	endOneTime(m_device, m_pools[m_frameIndex].graphics, cmd);
}

VkCommandBuffer VulkanApp::beginCommandsCompute()
{
	return beginOneTime(m_device, m_pools[m_frameIndex].compute);
}

void VulkanApp::endCommandsCompute(VkCommandBuffer cmd)
{
	endOneTime(m_device, m_pools[m_frameIndex].compute, cmd);
}

VkCommandBuffer VulkanApp::beginCommandsTransfer()
{
	return beginOneTime(m_device, m_pools[m_frameIndex].transfer);
}

void VulkanApp::endCommandsTransfer(VkCommandBuffer cmd)
{
	endOneTime(m_device, m_pools[m_frameIndex].transfer, cmd);
}

// ----------------- Submit helper -----------------

void VulkanApp::submit(
	VkQueue queue,
	const std::vector<VkCommandBuffer> &cmdBuffers,
	const std::vector<VkSemaphore> &waitSemaphores,
	const std::vector<VkPipelineStageFlags> &waitStages,
	const std::vector<VkSemaphore> &signalSemaphores,
	VkFence fence)
{
	if (waitSemaphores.size() != waitStages.size())
		throw std::runtime_error("submit: waitSemaphores and waitStages size mismatch");

	VkSubmitInfo si{VK_STRUCTURE_TYPE_SUBMIT_INFO};
	si.commandBufferCount = (uint32_t)cmdBuffers.size();
	si.pCommandBuffers = cmdBuffers.empty() ? nullptr : cmdBuffers.data();

	si.waitSemaphoreCount = (uint32_t)waitSemaphores.size();
	si.pWaitSemaphores = waitSemaphores.empty() ? nullptr : waitSemaphores.data();
	si.pWaitDstStageMask = waitStages.empty() ? nullptr : waitStages.data();

	si.signalSemaphoreCount = (uint32_t)signalSemaphores.size();
	si.pSignalSemaphores = signalSemaphores.empty() ? nullptr : signalSemaphores.data();

	vkCheck(vkQueueSubmit(queue, 1, &si, fence), "vkQueueSubmit failed");
}

// ----------------- End of frame advance -----------------

// NOTE: VulkanApp does not automatically present or advance frame index.
// You can choose to advance frame index after your app submits/presents.
// Common approach: derived app calls this at end of onVkFrame().