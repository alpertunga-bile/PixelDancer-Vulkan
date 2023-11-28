#pragma once

#include "debug.h"

#include <vector>
#include <functional>
#include <unordered_map>

#include "vma/vk_mem_alloc.h"

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
		std::vector<PxdQueueInfo> queue_infos;
		std::function<bool( VkPhysicalDevice )> physical_device_selector;
		VkPhysicalDeviceFeatures2 device_features;
	};

	struct PxdQueueSubmitInfo
	{
		VkPipelineStageFlags stage_flag;
		std::vector<VkSemaphore> wait_semaphores;
		std::vector<VkSemaphore> signal_semaphores;
		std::vector<VkCommandBuffer> commandbuffers;
		VkFence fence;
	};

	struct PxdQueuePresentInfo
	{
		std::vector<VkSwapchainKHR> swapchains;
		std::vector<VkSemaphore> wait_semaphores;
		uint32_t swapchain_image_index;
	};

	struct Queue
	{
		VkQueueFlags flag;
		uint32_t family_index = 0;
		uint32_t queue_index = 0;
		VkQueue queue = VK_NULL_HANDLE;

		void submit(PxdQueueSubmitInfo pqsi)
		{
				VkSubmitInfo si = {};
				si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				si.pNext = nullptr;

				si.pWaitDstStageMask = &pqsi.stage_flag;

				si.waitSemaphoreCount = pqsi.wait_semaphores.size();
				si.pWaitSemaphores = pqsi.wait_semaphores.data();

				si.signalSemaphoreCount = pqsi.signal_semaphores.size();
				si.pSignalSemaphores = pqsi.signal_semaphores.data();

				si.commandBufferCount = pqsi.commandbuffers.size();
				si.pCommandBuffers = pqsi.commandbuffers.data();

			VK_CHECK(
				vkQueueSubmit(queue, 1, &si, pqsi.fence)
			);
		}

		void present(PxdQueuePresentInfo pqpi)
		{
			VkPresentInfoKHR pi = {};
			pi.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			pi.pNext = nullptr;

			pi.swapchainCount = pqpi.swapchains.size();
			pi.pSwapchains = pqpi.swapchains.data();

			pi.waitSemaphoreCount = pqpi.wait_semaphores.size();
			pi.pWaitSemaphores = pqpi.wait_semaphores.data();

			pi.pImageIndices = &pqpi.swapchain_image_index;

			VK_CHECK(
				vkQueuePresentKHR(queue, &pi)
			);
		}
	};

	class Nexus
	{
	public:
		void init(PxdNexusCreateInfo create_info );
		void destroy();

		void add_instance_extension( const char* ext );
		void add_device_extension( const char* ext );
		void remove_instance_extension( const char* ext );
		void remove_device_extension( const char* ext );

		static uint32_t get_queue_family( VkQueueFlags queue_flag, VkPhysicalDevice physical_device );

		VkQueue get_queue( std::string queue_name );
		Queue get_queue_class(std::string queue_name);

		void print_physical_device_info();

		inline operator VkInstance() const
		{
			return m_instance;
		}

		inline operator VkDevice() const
		{
			return m_device;
		}

		inline operator VkPhysicalDevice() const
		{
			return m_physical_device;
		}

		inline VkSurfaceKHR get_surface()
		{
			return m_surface;
		}

		inline VkSurfaceKHR* get_surface_ptr()
		{
			return &m_surface;
		}

		inline VmaAllocator get_allocator()
		{
			return m_allocator;
		}

	private:
		void init_instance(const char* app_name, uint32_t api_version );
		void init_debugs();
		void init_physical_device(std::function<bool(VkPhysicalDevice)> selector);
		void add_queue_info( PxdQueueInfo info );
		void init_device( VkPhysicalDeviceFeatures2& device_features );
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
		VmaAllocator m_allocator = VK_NULL_HANDLE;

		// Debug
		VkDebugUtilsMessengerEXT m_messenger = VK_NULL_HANDLE;
		VkDebugReportCallbackEXT m_report_callback = VK_NULL_HANDLE;

		// Device
		VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
		VkDevice m_device = VK_NULL_HANDLE;

		VkSurfaceKHR m_surface = VK_NULL_HANDLE;

		// Queue
		std::vector<VkDeviceQueueCreateInfo> m_queue_create_infos;
		std::unordered_map<std::string, Queue> m_queue_map;
	};
}