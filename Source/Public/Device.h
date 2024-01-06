#pragma once
#include <string>
#include <vector>
#include <optional>
#include "Window.h"

namespace Application
{
	struct QueueFamiliesIndices
	{
		// std::optional allow us to hase .has_value() funct
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete()
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class Device
	{
	public:
		Device(Window& window);
		~Device();

		VkDevice GetDevice() { return device; }
		VkSurfaceKHR GetSurface() { return surface; }
		VkDevice GetLogicalDevice() { return device; }
		VkPhysicalDevice GetPhysicalDevice() { return physicalDevice; }

		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		QueueFamiliesIndices FindQueueFamilies(VkPhysicalDevice device);

#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif

	private:
		void CreateInstance();
		void CreateSurface();
		void ShowExtensions();
		std::vector<const char*> GetRequiredExtensions();
		bool CheckValidationLayerSupport();
		void SetupDebugMessenger();
		void Cleanup();

		void DestroyDebugUtilsMessengerEXT(
			VkInstance instance,
			VkDebugUtilsMessengerEXT callback,
			const VkAllocationCallbacks* pAllocator);

		VkResult CreateDebugUtilsMessengerEXT(
			VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pCallback);
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		void PickPhysicalDevice();
		bool isDeviceSuitable(VkPhysicalDevice);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

		void CreateLogicalDevice();

		const std::vector<const char*> validationLayers =
		{
			"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<const char*> deviceExtensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		Window& window;
		VkDebugUtilsMessengerEXT callback;
		VkInstance instance;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties physicalDeviceProperties;
		VkDevice device;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkSurfaceKHR surface;
	};
}