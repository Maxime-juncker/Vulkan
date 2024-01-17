#pragma once
#include "Device.h"


namespace Application
{
	class SwapChain
	{
	public:
		SwapChain(Device& device, Window& window);
		void Cleanup();

		void CreateSwapChain();
		void RecreateSwapChain();

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		uint32_t GetWidth() { return swapChainExtend.width; }
		uint32_t GetHeight() { return swapChainExtend.height; }

		VkRenderPass GetRenderPass() { return renderPass; }
		size_t ImageCount() { return swapChainImages.size(); }
		VkExtent2D GetSwapChainExtent() { return swapChainExtend; }

		VkFramebuffer GetFrameBuffer(int index) { return swapChainFramebuffers[index]; }
		VkFormat GetSwapChainImageFormat() { return swapChainImageFormat; }
		std::vector<VkFramebuffer> GetSwapChainFramebuffers() { return swapChainFramebuffers; }
		size_t GetImageCount() { return swapChainImages.size(); }
		VkImageView GetImageView(int index) { return swapChainImageViews[index]; }
		VkSwapchainKHR GetSwapChain() { return swapChain; }

	private:
		void CreateImageViews();
		void CreateRenderPass();
		void CreateFrameBuffers();
		void CleanupSwapChain();

		// Swap chain
		Device& device;
		Window& window;

		VkExtent2D windowExtent;
		VkSwapchainKHR swapChain;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtend;

		VkRenderPass renderPass;

		// Images view
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;

		// Frame buffers
		std::vector<VkFramebuffer> swapChainFramebuffers;
	};

}