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
		queue_info.name = "graphics";
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

		m_graphics_family = Nexus::get_queue_family( VK_QUEUE_GRAPHICS_BIT, m_nexus );
		m_graphics_queue = m_nexus.get_queue( "graphics" );

		int width, height;
		glfwGetWindowSize( window, &width, &height );

		PxdSwapchainInfo swapchain_info;
		swapchain_info.device = m_nexus;
		swapchain_info.physicalDevice = m_nexus;
		swapchain_info.graphics_family = m_graphics_family;
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
		pool_info.queue_family = m_graphics_family;

		m_commandpool.init( pool_info );

		// ---------------------------------------------------------------------------------------------------------------------
		// PXDVK CommandBuffer

		std::vector<PxdCommandBufferInfo> cmdbuffer_infos;
		PxdCommandBufferInfo main_cmd_info = {};
		main_cmd_info.name = "main";
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

		m_swapchain_framebuffers.resize( m_swapchain.get_image_count() );

		for ( int i = 0; i < m_swapchain.get_image_count(); i++ )
		{
			fbinfo.image_views.clear();
			fbinfo.image_views.push_back( m_swapchain.get_img_view( i ) );

			m_swapchain_framebuffers [ i ].init( fbinfo );
		}

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

		uint32_t swapchain_image_index;
		VK_CHECK(
			vkAcquireNextImageKHR(
				m_nexus, 
				m_swapchain,
				1000000000,
				m_present_semaphore.get(),
				nullptr,
				&swapchain_image_index )
		);

		m_commandpool [ "main" ].reset();
		m_commandpool [ "main" ].begin_recording( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );

		VkClearValue clear_value;
		float flash = std::abs( std::sin( frame_number / 120.0f ) );
		clear_value.color = { {0.0f, 0.0f, flash, 1.0f} };

		VkRenderPassBeginInfo rpbi = {};
		rpbi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		rpbi.pNext = nullptr;

		rpbi.renderPass = m_renderpass.get();
		rpbi.renderArea.offset.x = 0;
		rpbi.renderArea.offset.y = 0;
		rpbi.renderArea.extent = { .width = 1280, .height = 720 };
		rpbi.framebuffer = m_swapchain_framebuffers [ swapchain_image_index ].get();

		rpbi.clearValueCount = 1;
		rpbi.pClearValues = &clear_value;

		vkCmdBeginRenderPass(
			m_commandpool [ "main" ].get(), &rpbi, VK_SUBPASS_CONTENTS_INLINE
		);

		vkCmdEndRenderPass( m_commandpool [ "main" ].get() );

		VK_CHECK( vkEndCommandBuffer( m_commandpool [ "main" ].get() ) );

		VkSubmitInfo si = {};
		si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		si.pNext = nullptr;
		
		VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		si.pWaitDstStageMask = &wait_stage;

		si.waitSemaphoreCount = 1;
		si.pWaitSemaphores = m_present_semaphore.get_ptr();

		si.signalSemaphoreCount = 1;
		si.pSignalSemaphores = m_render_semaphore.get_ptr();

		si.commandBufferCount = 1;
		si.pCommandBuffers = m_commandpool [ "main" ].get_ptr();

		VK_CHECK(
			vkQueueSubmit( m_graphics_queue, 1, &si, m_render_fence.get() )
		);

		VkPresentInfoKHR pi = {};
		pi.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		pi.pNext = nullptr;

		pi.pSwapchains = m_swapchain.get_ptr();
		pi.swapchainCount = 1;

		pi.pWaitSemaphores = m_render_semaphore.get_ptr();
		pi.waitSemaphoreCount = 1;

		pi.pImageIndices = &swapchain_image_index;

		VK_CHECK(
			vkQueuePresentKHR( m_graphics_queue, &pi )
		);
	}

	void Application::destroy()
	{
		vkDeviceWaitIdle( m_nexus );

		m_render_fence.destroy();
		m_present_semaphore.destroy();
		m_render_fence.destroy();

		for ( Framebuffer fb : m_swapchain_framebuffers )
		{
			fb.destroy();
		}

		m_renderpass.destroy();
		m_commandpool.destroy();
		m_swapchain.destroy();
		m_nexus.destroy();
	}
}
