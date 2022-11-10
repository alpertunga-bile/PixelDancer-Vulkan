#include "../Include/Instance.h"

namespace pxd
{
	Instance::~Instance()
	{
		vkDestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);

		vkDestroyInstance(m_instance, nullptr);
	}

	void Instance::AddValidationLayer(const char* valLayer)
	{
		validationLayers.push_back(valLayer);
	}

	void Instance::AddExtension(const char* ext)
	{
		instanceExtensions.push_back(ext);
	}

	void Instance::CreateInstance()
	{
		VkApplicationInfo applicationInfo = {};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pApplicationName = "PixelDancer-Vulkan";
		applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.pEngineName = "PixelDancer";
		applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &applicationInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
		createInfo.ppEnabledExtensionNames = instanceExtensions.data();

#ifdef PXD_VK_ENABLE_VALIDATION_LAYERS
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#else
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
#endif

		VK_CHECK(vkCreateInstance(&createInfo, nullptr, &m_instance));
	}

	bool Instance::CheckValidationLayer()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		bool layerFound = false;
		for (const char* layerName : validationLayers)
		{
			layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				layerFound = true;
				break;
			}

			if (!layerFound) return false;
		}

		return true;
	}

	VkInstance* Instance::GetpInstance()
	{
		return &m_instance;
	}

	VkDebugUtilsMessengerEXT* Instance::GetpDebugMessenger()
	{
		return &m_debugMessenger;
	}
}