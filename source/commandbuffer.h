#pragma once

#include "debug.h"

namespace pxdvk
{
	class CommandBuffer
	{
	public:
		void init(VkDevice device, VkCommandPool commandpool, VkCommandBufferLevel level);
		void destroy();

		void reset();

		void begin_recording( VkCommandBufferUsageFlags usage_flags, VkCommandBufferInheritanceInfo* pInheritance = nullptr );
		void end_recording();

		void begin_renderpass(VkRenderPassBeginInfo* rpbi, VkSubpassContents subpass_contents = VK_SUBPASS_CONTENTS_INLINE);
		void end_renderpass();

		void bind_pipeline( VkPipelineBindPoint bind_point, VkPipeline pipeline );

		void draw( uint32_t vertex_count, uint32_t instance_count = 1, uint32_t first_vertex = 0, uint32_t first_instance = 0 );

		VkCommandBuffer get()
		{
			return m_commandbuffer;
		}

		VkCommandBuffer* get_ptr()
		{
			return &m_commandbuffer;
		}

	private:
		VkCommandBuffer m_commandbuffer = VK_NULL_HANDLE;
	};
}