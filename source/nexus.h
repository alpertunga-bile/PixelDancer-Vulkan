#pragma once

#include "debug.h"

#include <vector>
#include <functional>
#include <unordered_map>

namespace pxdvk
{
	struct PxdQueueInfo
	{
		std::string name = "graphics";
		VkQueueFlags flag = VK_QUEUE_GRAPHICS_BIT;
		uint32_t queue_index = 0;
		float priority = 1.0f;
	};

	struct PxdNexusCreateInfo
	{
		const char* app_name = "PixelDancer-Vulkan";
		uint32_t api_version = VK_API_VERSION_1_1;
		bool enable_raytracing = false;
		std::vector<PxdQueueInfo> queue_infos;
		std::function<bool( VkPhysicalDevice )> physical_device_selector;

		void add_info( PxdQueueInfo info )
		{
			queue_infos.push_back( info );
		}
	};

	struct PxdQueue
	{
		VkQueueFlags flag;
		uint32_t family_index;
		uint32_t queue_index;
		VkQueue queue;
	};

	class Nexus
	{
	public:
		void init(PxdNexusCreateInfo create_info );
		void destroy();

		void add_instance_extension( const char* ext );
		void remove_instance_extension( const char* ext );

		static uint32_t get_queue_family( VkQueueFlags queue_flag, VkPhysicalDevice physical_device );

		VkQueue get_queue( std::string queue_name );

		void print_physical_device_info();

		operator VkInstance() const
		{
			return m_instance;
		}

		operator VkDevice() const
		{
			return m_device;
		}

		operator VkPhysicalDevice() const
		{
			return m_physical_device;
		}

		VkSurfaceKHR get_surface()
		{
			return m_surface;
		}

		VkSurfaceKHR* get_surface_ptr()
		{
			return &m_surface;
		}

	private:
		void init_instance(const char* app_name, uint32_t api_version );
		void init_debugs();
		void init_physical_device(std::function<bool(VkPhysicalDevice)> selector);
		void add_queue_info( PxdQueueInfo info );
		void init_device( bool enable_raytracing );
		void init_queues();

	private:
		std::vector<const char*> m_validation_layers = { "VK_LAYER_KHRONOS_validation" };

		std::vector<const char*> m_instance_extensions = {
			"VK_KHR_surface",
		#if defined (_WIN32)
			"VK_KHR_win32_surface",
		#endif
		#if defined (__APPLE__)
			"VK_MVK_macos_surface",
		#endif
		#if defined(__linux__)
			"VK_KHR_xcb_surface",
			"VK_KHR_wayland_surface",
		#endif
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
		};

		std::vector<const char*> m_device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		// Instance
		VkInstance m_instance = VK_NULL_HANDLE;

		// Debug
		VkDebugUtilsMessengerEXT m_messenger = VK_NULL_HANDLE;
		VkDebugReportCallbackEXT m_report_callback = VK_NULL_HANDLE;

		// Device
		VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
		VkDevice m_device = VK_NULL_HANDLE;

		VkSurfaceKHR m_surface = VK_NULL_HANDLE;

		// Queue
		std::vector<VkDeviceQueueCreateInfo> m_queue_create_infos;
		std::unordered_map<std::string, PxdQueue> m_queue_map;
	};
}