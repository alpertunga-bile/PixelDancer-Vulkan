#pragma once

#include "debug.h"

#include <vector>

namespace pxdvk
{
	struct PxdFramebufferInfo
	{
		VkDevice device = VK_NULL_HANDLE;
		VkRenderPass renderpass = VK_NULL_HANDLE;
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t depth = 1;
		std::vector<VkImageView> image_views;
	};

	class Framebuffer
	{
	public:
		void init( PxdFramebufferInfo info );
		void destroy();

		VkFramebuffer get() const
		{
			return m_framebuffer;
		}

	private:
		VkDevice m_device = VK_NULL_HANDLE;

		VkFramebuffer m_framebuffer = VK_NULL_HANDLE;
	};
}