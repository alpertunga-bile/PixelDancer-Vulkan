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
		Queue m_graphics_queue;
		std::string m_graphics_queue_name = "graphics";

		Swapchain m_swapchain;

		CommandPool m_commandpool;
		std::string main_commandbuffer_name = "main";

		Renderpass m_renderpass;

		Semaphore m_render_semaphore;
		Semaphore m_present_semaphore;
		Fence m_render_fence;
	};
}