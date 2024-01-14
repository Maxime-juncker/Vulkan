#include "../Public/SwapChain.h"

#include "../Public/App.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <array>

namespace Application
{
	SwapChain::SwapChain(Device& device, Window& window) : device{device}, window{window}
	{
		windowExtent = window.GetExtend();
		CreateSwapChain();
		CreateImageView();
		CreateRenderPass();
		CreateFrameBuffers();
	}

	void SwapChain::Cleanup()
	{
		// Cleaning up the ressources
		CleanupSwapChain();
		vkDestroyRenderPass(device.GetDevice(), renderPass, nullptr);
	}

	void SwapChain::CreateSwapChain()
	{
		SwapChainSupportDetails swapChainSupport = device.QuerySwapChainSupport(device.GetPhysicalDevice());

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extend = ChooseSwapExtent(swapChainSupport.capabilities);

		// Getting the fps (0 = no max fps)
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		{
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		// Setting for the swap chain
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = device.GetSurface();

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extend;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamiliesIndices indices = device.FindQueueFamilies(device.GetPhysicalDevice());
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;		// Optional
			createInfo.pQueueFamilyIndices = nullptr;	// Optional
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // not a transparent window
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		// Creating the swap chain
		if (vkCreateSwapchainKHR(device.GetDevice(), &createInfo, nullptr, &swapChain) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create swap chain");
		}

		// Setting the images of the chain
		vkGetSwapchainImagesKHR(device.GetDevice(), swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device.GetDevice(), swapChain, &imageCount, swapChainImages.data());

		swapChainImageFormat = surfaceFormat.format;
		swapChainExtend = extend;
	}

	void SwapChain::CreateImageView()
	{
		swapChainImageViews.resize(swapChainImages.size());

		// Populating the swap chain images views
		for (size_t i = 0; i < swapChainImages.size(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapChainImages[i];

			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapChainImageFormat;

			// Setting the colors of the image view
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			// Setting image usecase
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			// Creating the image view
			if (vkCreateImageView(device.GetDevice(), &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create an image view");
			}
		}
	}

	void SwapChain::CreateRenderPass()
	{

		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; //VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		// Creating subpass dependencies
		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(device.GetDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create render pass");
		}
	}

	void SwapChain::CreateFrameBuffers()
	{
		swapChainFramebuffers.resize(swapChainImageViews.size());

		for (size_t i = 0; i < swapChainImageViews.size(); i++)
		{
			VkImageView attachment[] =
			{
				swapChainImageViews[i]
			};

			VkFramebufferCreateInfo framebuffersInfo{};
			framebuffersInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebuffersInfo.renderPass = renderPass;
			framebuffersInfo.attachmentCount = 1;
			framebuffersInfo.pAttachments = attachment;
			framebuffersInfo.width = GetWidth();
			framebuffersInfo.height = GetHeight();
			framebuffersInfo.layers = 1;

			if (vkCreateFramebuffer(device.GetDevice(), &framebuffersInfo, nullptr, &swapChainFramebuffers[i])
				!= VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create framebuffers");
			}
		}
	}

	void SwapChain::RecreateSwapChain()
	{
		// Hanldling window minimization
		int width = 0, height = 0;
		glfwGetFramebufferSize(window.GetWindow(), &width, &height);
		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(window.GetWindow(), &width, &height);
			glfwWaitEvents();
		}

		// Touching ressource while they are in use is a bad idea.
		vkDeviceWaitIdle(device.GetDevice());

		// Cleaning up everything before recreating a new swap chain.
		CleanupSwapChain();

		// Actually creating a swap chain.
		CreateSwapChain();
		CreateImageView();
		CreateFrameBuffers();
	}

	void SwapChain::CleanupSwapChain()
	{
		// Cleaning up the ressources

		for (auto framebuffer : swapChainFramebuffers)
		{
			vkDestroyFramebuffer(device.GetDevice(), framebuffer, nullptr);
		}

		for (auto imageView : swapChainImageViews)
		{
			vkDestroyImageView(device.GetDevice(), imageView, nullptr);
		}

		vkDestroySwapchainKHR(device.GetDevice(), swapChain, nullptr);
	}

	VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			// This is the best format, if it's supported by the gpu we take it
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
				availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}
		// We haven't found the srgb color so we return the first other format 
		return availableFormats[0];
	}

	VkPresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			// If we can found VK_PRESENT_MODE_MAILBOX_KHR, then we can use trible buffuring
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				std::cout << "[INFO]: Present mode selected: Mailbox" << std::endl;
				return availablePresentMode;
			}
		}
		std::cout << "[INFO]: Present mode: vsync (mailbox not available)" << std::endl;
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		// Note: ChooseSwapExtent return the resolution of the images in the swap chain
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}
		else
		{
			VkExtent2D actualExtent = windowExtent;

			// Getting the biggest resolution possible that's in the range of the width and height of the app 
			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
				capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
				capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}

}