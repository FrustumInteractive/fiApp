#include "app.h"

#include "triangle.vert.spv.h"
#include "triangle.frag.spv.h"

#include <cstring>
#include <stdexcept>

namespace
{
void checkVk(VkResult result, const char *message)
{
	if (result != VK_SUCCESS)
		throw std::runtime_error(message);
}

VkShaderModule createShader(VkDevice device, const unsigned char *bytes, size_t byteCount)
{
	std::vector<uint32_t> words((byteCount + sizeof(uint32_t) - 1) / sizeof(uint32_t), 0);
	std::memcpy(words.data(), bytes, byteCount);

	VkShaderModuleCreateInfo info{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
	info.codeSize = byteCount;
	info.pCode = words.data();

	VkShaderModule module = VK_NULL_HANDLE;
	checkVk(vkCreateShaderModule(device, &info, nullptr, &module), "vkCreateShaderModule failed");
	return module;
}
}

MyApp::MyApp(const int argc, const char *argv[])
	: VulkanApp(argc, argv)
{
}

MyApp::~MyApp()
{
}

void MyApp::onKeyPress(FI::Event e)
{
	const eKeyCode key = (eKeyCode)e.data_uint32()[0];
	if (key == KEY_ESC || key == KEY_Q)
		m_bQuit = true;
}

void MyApp::onKeyRelease(FI::Event e)
{
	const eKeyCode key = (eKeyCode)e.data_uint32()[0];
	if (key == KEY_W || key == KEY_A || key == KEY_S || key == KEY_D)
		FI::LOG("Key released: ", (unsigned int)key);
}

void MyApp::onMouseLeftClick(FI::Event e)
{
	FI::LOG("LMB click (", e.data_float()[0], ",", e.data_float()[1], ")");
}

void MyApp::onMouseLeftRelease(FI::Event e)
{
	FI::LOG("LMB release (", e.data_float()[0], ",", e.data_float()[1], ")");
}

void MyApp::onMouseRightClick(FI::Event e)
{
	FI::LOG("RMB click (", e.data_float()[0], ",", e.data_float()[1], ")");
}

void MyApp::onMouseRightRelease(FI::Event e)
{
	FI::LOG("RMB release (", e.data_float()[0], ",", e.data_float()[1], ")");
}

void MyApp::initScene()
{
	FI::LOG("Initializing Vulkan triangle scene");
	createRenderResources();
	createSyncObjects();
}

void MyApp::deinitScene()
{
	destroySyncObjects();
	destroyRenderResources();
}

void MyApp::onSwapchainDestroying()
{
	destroyRenderResources();
}

void MyApp::onSwapchainCreated()
{
	createRenderResources();
}

void MyApp::createRenderResources()
{
	VkDevice device = vkDevice();

	VkAttachmentDescription color{};
	color.format = swapchainFormat();
	color.samples = VK_SAMPLE_COUNT_1_BIT;
	color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorRef{};
	colorRef.attachment = 0;
	colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &color;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;
	checkVk(vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_renderPass), "vkCreateRenderPass failed");

	VkShaderModule vertexShader = createShader(device, triangle_vert_spv, triangle_vert_spv_len);
	VkShaderModule fragmentShader = createShader(device, triangle_frag_spv, triangle_frag_spv_len);

	VkPipelineShaderStageCreateInfo stages[2]{};
	stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	stages[0].module = vertexShader;
	stages[0].pName = "main";
	stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	stages[1].module = fragmentShader;
	stages[1].pName = "main";

	VkPipelineVertexInputStateCreateInfo vertexInput{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	const VkExtent2D extent = swapchainExtent();
	VkViewport viewport{};
	viewport.width = (float)extent.width;
	viewport.height = (float)extent.height;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{{0, 0}, extent};

	VkPipelineViewportStateCreateInfo viewportState{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.lineWidth = 1.0f;

	VkPipelineMultisampleStateCreateInfo multisampling{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState blendAttachment{};
	blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	VkPipelineColorBlendStateCreateInfo blending{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
	blending.attachmentCount = 1;
	blending.pAttachments = &blendAttachment;

	VkPipelineLayoutCreateInfo layoutInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
	checkVk(vkCreatePipelineLayout(device, &layoutInfo, nullptr, &m_pipelineLayout), "vkCreatePipelineLayout failed");

	VkGraphicsPipelineCreateInfo pipelineInfo{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = stages;
	pipelineInfo.pVertexInputState = &vertexInput;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &blending;
	pipelineInfo.layout = m_pipelineLayout;
	pipelineInfo.renderPass = m_renderPass;
	checkVk(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline),
		"vkCreateGraphicsPipelines failed");

	vkDestroyShaderModule(device, fragmentShader, nullptr);
	vkDestroyShaderModule(device, vertexShader, nullptr);

	m_framebuffers.resize(swapchainImageCount());
	for (uint32_t i = 0; i < swapchainImageCount(); ++i)
	{
		VkImageView attachment = swapchainImageView(i);
		VkFramebufferCreateInfo framebufferInfo{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
		framebufferInfo.renderPass = m_renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = &attachment;
		framebufferInfo.width = extent.width;
		framebufferInfo.height = extent.height;
		framebufferInfo.layers = 1;
		checkVk(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_framebuffers[i]),
			"vkCreateFramebuffer failed");
	}
}

void MyApp::destroyRenderResources()
{
	VkDevice device = vkDevice();
	if (!device)
		return;
	for (VkFramebuffer framebuffer : m_framebuffers)
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	m_framebuffers.clear();
	if (m_pipeline) vkDestroyPipeline(device, m_pipeline, nullptr);
	if (m_pipelineLayout) vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
	if (m_renderPass) vkDestroyRenderPass(device, m_renderPass, nullptr);
	m_pipeline = VK_NULL_HANDLE;
	m_pipelineLayout = VK_NULL_HANDLE;
	m_renderPass = VK_NULL_HANDLE;
}

void MyApp::createSyncObjects()
{
	m_imageAvailable.resize(maxFramesInFlight());
	m_renderFinished.resize(maxFramesInFlight());
	m_inFlight.resize(maxFramesInFlight());

	VkSemaphoreCreateInfo semaphoreInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
	VkFenceCreateInfo fenceInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	for (uint32_t i = 0; i < maxFramesInFlight(); ++i)
	{
		checkVk(vkCreateSemaphore(vkDevice(), &semaphoreInfo, nullptr, &m_imageAvailable[i]), "vkCreateSemaphore failed");
		checkVk(vkCreateSemaphore(vkDevice(), &semaphoreInfo, nullptr, &m_renderFinished[i]), "vkCreateSemaphore failed");
		checkVk(vkCreateFence(vkDevice(), &fenceInfo, nullptr, &m_inFlight[i]), "vkCreateFence failed");
	}
}

void MyApp::destroySyncObjects()
{
	VkDevice device = vkDevice();
	for (VkFence fence : m_inFlight) if (fence) vkDestroyFence(device, fence, nullptr);
	for (VkSemaphore semaphore : m_renderFinished) if (semaphore) vkDestroySemaphore(device, semaphore, nullptr);
	for (VkSemaphore semaphore : m_imageAvailable) if (semaphore) vkDestroySemaphore(device, semaphore, nullptr);
	m_inFlight.clear();
	m_renderFinished.clear();
	m_imageAvailable.clear();
}

void MyApp::drawScene()
{
	const uint32_t frameIndex = frame().frameIndex;
	VkDevice device = vkDevice();
	checkVk(vkWaitForFences(device, 1, &m_inFlight[frameIndex], VK_TRUE, UINT64_MAX), "vkWaitForFences failed");

	uint32_t imageIndex = 0;
	VkResult acquired = vkAcquireNextImageKHR(device, vkSwapchain(), UINT64_MAX,
		m_imageAvailable[frameIndex], VK_NULL_HANDLE, &imageIndex);
	if (acquired == VK_ERROR_OUT_OF_DATE_KHR)
	{
		requestSwapchainRecreate();
		return;
	}
	if (acquired == VK_SUBOPTIMAL_KHR)
		requestSwapchainRecreate();
	else
		checkVk(acquired, "vkAcquireNextImageKHR failed");

	checkVk(vkResetFences(device, 1, &m_inFlight[frameIndex]), "vkResetFences failed");
	resetCurrentFrameCommandPools();
	VkCommandBuffer command = beginCommandsGraphics();

	VkClearValue clear{};
	clear.color = {{0.02f, 0.04f, 0.08f, 1.0f}};
	VkRenderPassBeginInfo passInfo{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
	passInfo.renderPass = m_renderPass;
	passInfo.framebuffer = m_framebuffers[imageIndex];
	passInfo.renderArea.extent = swapchainExtent();
	passInfo.clearValueCount = 1;
	passInfo.pClearValues = &clear;

	vkCmdBeginRenderPass(command, &passInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
	vkCmdDraw(command, 3, 1, 0, 0);
	vkCmdEndRenderPass(command);
	endCommandsGraphics(command);

	const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	submit(queues().graphics, {command}, {m_imageAvailable[frameIndex]}, {waitStage},
		{m_renderFinished[frameIndex]}, m_inFlight[frameIndex]);
	setCurrentSwapchainImageIndex(imageIndex);
	present({m_renderFinished[frameIndex]});
}
