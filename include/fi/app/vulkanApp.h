// vulkanApp.h
/*
 * vulkanApp.h
 *
 *  Created on: Feb 19, 2026
 *      Author: Roger Dass
 *      Copyright Frustum Interactive Inc. - All rights reserved.
 *
 *  Notes:
 *  - VulkanApp owns platform/device/swapchain/frame orchestration (optional present).
 *  - Derived apps implement onVkInit/onVkFrame/onVkShutdown and record/submit work as needed.
 *  - This is intentionally not a "renderer". It exposes queues + helpers and stays general.
 */

#ifndef _VULKANAPP_H
#define _VULKANAPP_H

#include <vulkan/vulkan.h>

#if defined(WIN32)
	#include "win32App.h"
	class VulkanApp : public Win32App
#elif defined(WEB)
	#include "webApp.h"
	class VulkanApp : public WebApp
#elif defined(LINUX)
	#include "x11App.h"
#elif defined(OSX)
	#include "osxApp.h"
	#include <vulkan/vulkan_metal.h>
	class VulkanApp : public OSXApp
#elif defined(__ANDROID__)
	#include "mobileApp.h"
	class VulkanApp : public MobileApp
#else
	#ifdef __APPLE__
		#include "TargetConditionals.h"
		#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
			#include "mobileApp.h"
			class VulkanApp : public MobileApp
		#elif TARGET_OS_MAC
			#include "osxApp.h"
			#include <vulkan/vulkan_metal.h>
			class VulkanApp : public OSXApp
		#else
			#error "Unknown Apple platform"
		#endif
	#else
		#error "Unknown platform"
	#endif
#endif
{
public:
	VulkanApp(const int argc = 0, const char *argv[] = 0);
	virtual ~VulkanApp();

	// ---- High-level lifecycle hooks for derived apps ----
	// Called after VkDevice (and optionally swapchain) is created.
	virtual void initScene() {}

	// Called every frame, after beginFrame() succeeded.
	// App may record and submit multiple command buffers and use any queue(s).
	virtual void drawScene() {}

	// Called during shutdown before Vulkan objects are destroyed.
	virtual void deinitScene() {}

	// ---- Application overrides ----
	void resize(int width, int height) override;
	void gfxAPIInit() override;
	void gfxAPIDraw() override;
	void gfxAPIDeinit() override;

	// ---- Presentation mode ----
	// Default: present to window swapchain.
	// Derived app can call setPresentEnabled(false) before createWindow() to run headless/offscreen.
	void setPresentEnabled(bool enabled);
	bool isPresentEnabled() const;

	// ---- Accessors (read-only handles) ----
	struct Queues
	{
		// Some may alias the same queue family and even the same VkQueue.
		VkQueue graphics = VK_NULL_HANDLE;
		VkQueue present = VK_NULL_HANDLE; // only valid if presentEnabled==true and supported
		VkQueue compute = VK_NULL_HANDLE;
		VkQueue transfer = VK_NULL_HANDLE;

		uint32_t graphicsFamily = UINT32_MAX;
		uint32_t presentFamily = UINT32_MAX;
		uint32_t computeFamily = UINT32_MAX;
		uint32_t transferFamily = UINT32_MAX;
	};

	VkInstance vkInstance() const { return m_instance; }
	VkPhysicalDevice vkPhysicalDevice() const { return m_phys; }
	VkDevice vkDevice() const { return m_device; }
	VkSurfaceKHR vkSurface() const { return m_surface; }	   // VK_NULL_HANDLE if not presenting
	VkSwapchainKHR vkSwapchain() const { return m_swapchain; } // VK_NULL_HANDLE if not presenting

	const Queues &queues() const { return m_queues; }

	VkFormat swapchainFormat() const { return m_swapchainFormat; }
	VkExtent2D swapchainExtent() const { return m_swapchainExtent; }
	uint32_t swapchainImageCount() const { return (uint32_t)m_swapchainImages.size(); }
	VkImageView swapchainImageView(uint32_t i) const { return m_swapchainImageViews[i]; }

	// ---- Frame info ----
	struct FrameContext
	{
		uint32_t frameIndex = 0; // 0..MAX_FRAMES_IN_FLIGHT-1
		bool presenting = false;

		uint32_t imageIndex = 0;					 // valid only if presenting
		VkSemaphore imageAvailable = VK_NULL_HANDLE; // valid only if presenting
		VkFence frameFence = VK_NULL_HANDLE;

		// convenience
		VkDevice device = VK_NULL_HANDLE;
		Queues queues{};
	};

	const FrameContext &frame() const { return m_frame; }

	// ---- Command buffer helpers ----
	// Allocate one-time command buffer from a given pool (graphics/compute/transfer).
	// These are per-frame pools so you can reset every frame cleanly.
	VkCommandBuffer beginCommandsGraphics();
	void endCommandsGraphics(VkCommandBuffer cmd);

	VkCommandBuffer beginCommandsCompute();
	void endCommandsCompute(VkCommandBuffer cmd);

	VkCommandBuffer beginCommandsTransfer();
	void endCommandsTransfer(VkCommandBuffer cmd);

	// ---- Submission helper ----
	// Thin wrapper over vkQueueSubmit with vectors.
	// Keeps VulkanApp general: app decides how many submits, which queues, and which semaphores.
	void submit(
		VkQueue queue,
		const std::vector<VkCommandBuffer> &cmdBuffers,
		const std::vector<VkSemaphore> &waitSemaphores,
		const std::vector<VkPipelineStageFlags> &waitStages,
		const std::vector<VkSemaphore> &signalSemaphores,
		VkFence fence = VK_NULL_HANDLE);

	// ---- Presentation helper (optional) ----
	// Present using the provided wait semaphores (typically "render finished" semaphores).
	// If presentEnabled==false this is a no-op.
	void present(const std::vector<VkSemaphore> &waitSemaphores);

protected:
	// Derived apps can call these if they want manual control, but normally you just implement onVkFrame().
	bool beginFrame();											// acquires image if presenting; sets m_frame fields; returns false on out-of-date
	void endFrame(const std::vector<VkSemaphore> &presentWait); // present if enabled

	// If you need to force swapchain recreate (e.g. after resize)
	void requestSwapchainRecreate();

private:
	// ---- internal init/cleanup ----
	void createInstance();
	void createSurface(); // only if presentEnabled
	void pickPhysicalDevice();
	void createDeviceAndQueues();

	void createSwapchain();	 // only if presentEnabled
	void destroySwapchain(); // only if presentEnabled
	void recreateSwapchainIfNeeded();

	void createCommandPools();
	void destroyCommandPools();

	void createFrameSync();
	void destroyFrameSync();

	// ---- helpers ----
	uint32_t findQueueFamily(VkQueueFlags required, VkQueueFlags excluded = 0) const;
	uint32_t findPresentFamily() const;

	void resetPerFramePools(uint32_t frameIndex);

private:
	// Toggle present/swapchain management
	bool m_presentEnabled = true;
	bool m_swapchainRecreateRequested = false;

	// Core Vulkan objects
	VkInstance m_instance = VK_NULL_HANDLE;
	VkSurfaceKHR m_surface = VK_NULL_HANDLE;
	VkPhysicalDevice m_phys = VK_NULL_HANDLE;
	VkDevice m_device = VK_NULL_HANDLE;

	Queues m_queues{};

	// Swapchain (optional)
	VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
	VkFormat m_swapchainFormat{};
	VkExtent2D m_swapchainExtent{};
	std::vector<VkImage> m_swapchainImages;
	std::vector<VkImageView> m_swapchainImageViews;

	// Per-frame command pools (one pool per queue type per frame)
	static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2; // can make 3 later
	struct PerFramePools
	{
		VkCommandPool graphics = VK_NULL_HANDLE;
		VkCommandPool compute = VK_NULL_HANDLE;
		VkCommandPool transfer = VK_NULL_HANDLE;
	};
	std::array<PerFramePools, MAX_FRAMES_IN_FLIGHT> m_pools{};

	// Per-frame sync
	std::array<VkFence, MAX_FRAMES_IN_FLIGHT> m_inFlightFences{};
	std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> m_imageAvailable{};
	std::vector<VkFence> imagesInFlight; // per swapchain image fence

	uint32_t m_frameIndex = 0;
	FrameContext m_frame{};
};

#endif /* _VULKANAPP_H */