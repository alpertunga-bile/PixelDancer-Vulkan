#include "../Include/Instance.hpp"

namespace pxd
{
    Instance::~Instance()
	{
		vkDestroyInstance(m_instance, nullptr);
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
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        #else
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
        #endif

		VK_CHECK(vkCreateInstance(&createInfo, nullptr, &m_instance));
	}

    VkInstance* Instance::GetpInstance()
	{
		return &m_instance;
	}
}