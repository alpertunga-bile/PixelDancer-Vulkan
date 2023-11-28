#pragma once

#include "nexus.h"
#include "swapchain.h"
#include "commandpool.h"
#include "renderpass.h"
#include "framebuffer.h"
#include "synchronize.h"
#include "pipeline.h"
#include "mesh.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#define FRAME_OVERLAP 3

namespace pxdvk
{
	enum class PxdAppType
	{
		PXD_AT_RASTERIZER,
		PXD_AT_RAYTRACING,
		PXD_AT_COMPUTE
	};

	class Application
	{
	private:
		struct FrameData
		{
			Semaphore present_semaphore, render_semaphore;
			Fence render_fence;

			CommandPool commandpool;
			std::string commandbuffer_name = "main";
		};

	public:
		void initialize(GLFWwindow* window, PxdAppType app_type);
		void render();
		void destroy();

	private:
		inline FrameData& get_current_frame()
		{
			return m_frames [ frame_number % FRAME_OVERLAP ];
		}

	protected:
		Nexus m_nexus;
		Queue m_graphics_queue;
		std::string m_graphics_queue_name = "graphics";

		Swapchain m_swapchain;

		Renderpass m_renderpass;

		Pipeline m_pipeline;
		PipelineLayout m_pipeline_layout;

		Mesh triangle_mesh;

		FrameData m_frames [ FRAME_OVERLAP ];

	private:
		uint32_t frame_number = 0;
	};
}