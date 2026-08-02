#ifndef FI_APP_VULKAN_TEST_APP_H
#define FI_APP_VULKAN_TEST_APP_H

#include "fi/app/vulkanApp.h"

class MyApp : public VulkanApp
{
public:
	MyApp(const int argc = 0, const char *argv[] = nullptr);
	~MyApp() override;

	void initScene() override;
	void drawScene() override;
	void deinitScene() override;

	void onKeyPress(FI::Event e) override;
	void onKeyRelease(FI::Event e) override;
	void onMouseLeftClick(FI::Event e) override;
	void onMouseLeftRelease(FI::Event e) override;
	void onMouseRightClick(FI::Event e) override;
	void onMouseRightRelease(FI::Event e) override;

protected:
	void onSwapchainDestroying() override;
	void onSwapchainCreated() override;

private:
	void createRenderResources();
	void destroyRenderResources();
	void createSyncObjects();
	void destroySyncObjects();

	VkRenderPass m_renderPass = VK_NULL_HANDLE;
	VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
	VkPipeline m_pipeline = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> m_framebuffers;
	std::vector<VkSemaphore> m_imageAvailable;
	std::vector<VkSemaphore> m_renderFinished;
	std::vector<VkFence> m_inFlight;
};

#endif
