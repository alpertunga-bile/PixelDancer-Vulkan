#include "application.h"

#include "shader.h"

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

	void Application::initialize(GLFWwindow* window, PxdAppType app_type)
	{
		// ---------------------------------------------------------------------------------------------------------------------
		// PXDVK NEXUS

		PxdNexusCreateInfo nexus_create_info = {};
		nexus_create_info.physical_device_selector = physical_device_selector;
		nexus_create_info.device_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		nexus_create_info.device_features.pNext = nullptr;

		if ( app_type == PxdAppType::PXD_AT_RAYTRACING )
		{
			m_nexus.add_device_extension( VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME );
			m_nexus.add_device_extension( VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME );
			m_nexus.add_device_extension( VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME );
			m_nexus.add_device_extension( VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME );
			m_nexus.add_device_extension( VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME );
			m_nexus.add_device_extension( VK_KHR_SPIRV_1_4_EXTENSION_NAME );
			m_nexus.add_device_extension( VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME );

			VkPhysicalDeviceBufferDeviceAddressFeatures address_features = {};
			address_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
			address_features.bufferDeviceAddress = VK_TRUE;

			VkPhysicalDeviceRayTracingPipelineFeaturesKHR pipeline_features = {};
			pipeline_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
			pipeline_features.rayTracingPipeline = VK_TRUE;
			pipeline_features.pNext = &address_features;

			VkPhysicalDeviceAccelerationStructureFeaturesKHR accel_features = {};
			accel_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
			accel_features.accelerationStructure = VK_TRUE;
			accel_features.pNext = &pipeline_features;

			nexus_create_info.device_features.pNext = &accel_features;
		}

		PxdQueueInfo queue_info;
		queue_info.name = m_graphics_queue_name;
		queue_info.queue_index = 0;
		queue_info.flag = VK_QUEUE_GRAPHICS_BIT;
		queue_info.priority = 1.0f;
		nexus_create_info.queue_infos.push_back( queue_info );

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

		// ---------------------------------------------------------------------------------------------------------------------
		// PXDVK Pipeline
		triangle_mesh.init( 3 );

		triangle_mesh[ 0 ].pos = { 1.f, 1.f, 0.0f };
		triangle_mesh[ 1 ].pos = { -1.f, 1.f, 0.0f };
		triangle_mesh[ 2 ].pos = { 0.f,-1.f, 0.0f };

		triangle_mesh [ 0 ].color = { 1.0f, 1.0f, 1.0f };
		triangle_mesh [ 1 ].color = { 1.0f, 1.0f, 1.0f };
		triangle_mesh [ 2 ].color = { 1.0f, 1.0f, 1.0f };

		triangle_mesh.upload( m_nexus.get_allocator() );

		std::vector<VkDescriptorSetLayout> descriptor_set_layouts;
		std::vector<VkPushConstantRange> push_constant_ranges;
		std::vector<VkPipelineColorBlendAttachmentState> color_blend_states;

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = 
			VK_COLOR_COMPONENT_R_BIT | 
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT | 
			VK_COLOR_COMPONENT_A_BIT;

		colorBlendAttachment.blendEnable = VK_FALSE;

		color_blend_states.push_back( colorBlendAttachment );

		m_pipeline_layout.init( m_nexus, descriptor_set_layouts, push_constant_ranges);

		m_pipeline.set_viewport(
			0.0f, 0.0f, ( float ) width, ( float ) height, 0.0f, 1.0f
		);

		m_pipeline.set_scissor( 0, 1, width, height );

		Shader vertex_shader;
		vertex_shader.init( m_nexus, "shaders/triangle_buffer.vert" );

		Shader fragment_shader;
		fragment_shader.init( m_nexus, "shaders/triangle_buffer.frag" );

		VertexInputDescription input_desc = triangle_mesh.get_input_desc();

		m_pipeline.add_shader( VK_SHADER_STAGE_VERTEX_BIT, vertex_shader.get() );
		m_pipeline.add_shader( VK_SHADER_STAGE_FRAGMENT_BIT, fragment_shader.get() );
		m_pipeline.create_vertex_input( input_desc.bindings, input_desc.attributes );
		m_pipeline.create_assembly_info( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST );
		m_pipeline.create_viewport_state();
		m_pipeline.create_rasterization( VK_POLYGON_MODE_FILL );
		m_pipeline.create_multisampling_state();
		m_pipeline.create_color_blend( color_blend_states );

		m_pipeline.init( m_nexus, m_renderpass.get(), m_pipeline_layout.get() );

		vertex_shader.destroy();
		fragment_shader.destroy();
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

		m_commandpool [ main_commandbuffer_name ].bind_pipeline( VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.get() );
		
		triangle_mesh.draw( m_commandpool [ main_commandbuffer_name ].get() );

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

		triangle_mesh.destroy();

		m_pipeline_layout.destroy();
		m_pipeline.destroy();

		m_renderpass.destroy();
		m_commandpool.destroy();
		m_swapchain.destroy();
		m_nexus.destroy();
	}
}
