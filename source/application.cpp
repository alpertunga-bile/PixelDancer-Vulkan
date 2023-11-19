#include "application.h"

namespace pxdvk
{
	bool physical_device_selector( VkPhysicalDevice physical_device )
	{
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties( physical_device, &properties );

		VkPhysicalDeviceFeatures features;
		vkGetPhysicalDeviceFeatures( physical_device, &features );

		const bool isDiscreteGPU = properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
		const bool isIntegratedGPU = properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
		const bool isGPU = isDiscreteGPU && isIntegratedGPU;

		return isDiscreteGPU && features.geometryShader;
	}

	void Application::initialize(GLFWwindow* window)
	{
		// ---------------------------------------------------------------------------------------------------------------------
		// PXDVK NEXUS

		PxdNexusCreateInfo nexus_create_info = {};
		nexus_create_info.physical_device_selector = physical_device_selector;
		nexus_create_info.enable_raytracing = false;

		PxdQueueInfo queue_info;
		queue_info.name = m_graphics_queue_name;
		queue_info.queue_index = 0;
		queue_info.flag = VK_QUEUE_GRAPHICS_BIT;
		queue_info.priority = 1.0f;
		nexus_create_info.add_info( queue_info );

		m_nexus.init(nexus_create_info);

		glfwCreateWindowSurface( m_nexus, window, nullptr, m_nexus.get_surface_ptr() );

#if defined (_DEBUG)
		m_nexus.print_physical_device_info();
#endif
		// ---------------------------------------------------------------------------------------------------------------------
		// PXDVK SWAPCHAIN

		m_graphics_queue = m_nexus.get_queue_class(m_graphics_queue_name);

		int width, height;
		glfwGetWindowSize( window, &width, &height );

		PxdSwapchainInfo swapchain_info;
		swapchain_info.device = m_nexus;
		swapchain_info.physicalDevice = m_nexus;
		swapchain_info.graphics_family = m_graphics_queue.family_index;
		swapchain_info.width = width;
		swapchain_info.height = height;
		swapchain_info.surface = m_nexus.get_surface();
		swapchain_info.surface_format = { .format = VK_FORMAT_B8G8R8A8_UNORM, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

		m_swapchain.init( swapchain_info );

		// ---------------------------------------------------------------------------------------------------------------------
		// PXDVK CommandPool

		PxdCommandPoolInfo pool_info = {};
		pool_info.device = m_nexus;
		pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		pool_info.queue_family = m_graphics_queue.family_index;

		m_commandpool.init( pool_info );

		// ---------------------------------------------------------------------------------------------------------------------
		// PXDVK CommandBuffer

		std::vector<PxdCommandBufferInfo> cmdbuffer_infos;

		PxdCommandBufferInfo main_cmd_info = {};
		main_cmd_info.name = main_commandbuffer_name;
		main_cmd_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		
		cmdbuffer_infos.push_back( main_cmd_info );

		m_commandpool.allocate( cmdbuffer_infos );

		// ---------------------------------------------------------------------------------------------------------------------
		// PXDVK Renderpass

		m_renderpass.init( m_nexus, m_swapchain.get_image_format(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );
	
		// ---------------------------------------------------------------------------------------------------------------------
		// PXDVK Framebuffer

		PxdFramebufferInfo fbinfo;
		fbinfo.width = width;
		fbinfo.height = height;
		fbinfo.depth = 1;
		fbinfo.device = m_nexus;
		fbinfo.renderpass = m_renderpass.get();

		m_swapchain.create_framebuffers(fbinfo);

		// ---------------------------------------------------------------------------------------------------------------------
		// PXDVK Synchronization

		m_render_semaphore.init( m_nexus );
		m_present_semaphore.init( m_nexus );
		m_render_fence.init( m_nexus, VK_FENCE_CREATE_SIGNALED_BIT );
	}

	void Application::render( uint32_t frame_number )
	{
		m_render_fence.wait();
		m_render_fence.reset();

		uint32_t swapchain_image_index = m_swapchain.get_image_index(1, m_present_semaphore.get());

		m_commandpool [main_commandbuffer_name].reset();
		m_commandpool [main_commandbuffer_name].begin_recording( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );

		VkClearValue clear_value;
		float flash = std::abs( std::sin( frame_number / 360.0f ) );
		clear_value.color = { {0.0f, 0.0f, flash, 1.0f} };

		VkRenderPassBeginInfo rpbi = {};
		rpbi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		rpbi.pNext = nullptr;

		rpbi.renderPass = m_renderpass.get();
		rpbi.renderArea.offset = { .x = 0, .y = 0 };
		rpbi.renderArea.extent = { .width = 1280, .height = 720 };
		rpbi.framebuffer = m_swapchain.get_framebuffer(swapchain_image_index);

		rpbi.clearValueCount = 1;
		rpbi.pClearValues = &clear_value;

		m_commandpool[main_commandbuffer_name].begin_renderpass(&rpbi);

		m_commandpool[main_commandbuffer_name].end_renderpass();

		m_commandpool[main_commandbuffer_name].end_recording();

		PxdQueueSubmitInfo pqsi = {};
		pqsi.stage_flag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		pqsi.wait_semaphores.push_back(m_present_semaphore.get());
		pqsi.signal_semaphores.push_back(m_render_semaphore.get());
		pqsi.commandbuffers.push_back(m_commandpool[main_commandbuffer_name].get());
		pqsi.fence = m_render_fence.get();

		m_graphics_queue.submit(pqsi);

		PxdQueuePresentInfo pqpi = {};
		pqpi.swapchains.push_back(m_swapchain);
		pqpi.wait_semaphores.push_back(m_render_semaphore.get());
		pqpi.swapchain_image_index = swapchain_image_index;

		m_graphics_queue.present(pqpi);
	}

	void Application::destroy()
	{
		vkDeviceWaitIdle( m_nexus );

		m_render_fence.destroy();
		m_present_semaphore.destroy();
		m_render_semaphore.destroy();

		m_renderpass.destroy();
		m_commandpool.destroy();
		m_swapchain.destroy();
		m_nexus.destroy();
	}
}
