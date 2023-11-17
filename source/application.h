#pragma once

#include "nexus.h"
#include "swapchain.h"
#include "commandpool.h"
#include "renderpass.h"
#include "framebuffer.h"
#include "synchronize.h"

#include "GLFW/glfw3.h"

namespace pxdvk
{
	class Application
	{
	public:
		void initialize(GLFWwindow* window);
		void render( uint32_t frame_number );
		void destroy();

	protected:
		Nexus m_nexus;
		
		uint32_t m_graphics_family;
		VkQueue m_graphics_queue;

		Swapchain m_swapchain;
		std::vector<Framebuffer> m_swapchain_framebuffers;

		CommandPool m_commandpool;

		Renderpass m_renderpass;

		Semaphore m_render_semaphore;
		Semaphore m_present_semaphore;
		Fence m_render_fence;
	};
}