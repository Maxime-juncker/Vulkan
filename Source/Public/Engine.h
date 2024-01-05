#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <optional>

namespace Application
{
	struct QueueFamiliesIndices
	{
		// std::optional allow us to hase .has_value() funct
		std::optional<uint32_t> graphicsFamily;

		bool isComplete()
		{
			return graphicsFamily.has_value();
		}

	};

	class Engine
	{
	public:
		Engine();
		~Engine();

#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif

	private:
		void InitVulkan();
		void CreateInstance();
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
		QueueFamiliesIndices FindQueueFamilies(VkPhysicalDevice device);

		void CreateLogicalDevice();

		const std::vector<const char*> validationLayers =
		{
			"VK_LAYER_KHRONOS_validation"
		};

		VkDebugUtilsMessengerEXT callback;
		VkInstance instance;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties physicalDeviceProperties;
		VkDevice device;
		VkQueue graphicsQueue;
	};
}