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

		VkCommandBuffer get()
		{
			return m_commandbuffer;
		}

		VkCommandBuffer* get_ptr()
		{
			return &m_commandbuffer;
		}

	private:
		VkCommandBuffer m_commandbuffer;
	};
}