#pragma once

#include "debug.h"

#include <vector>

namespace pxdvk
{
	struct PxdFramebufferInfo
	{
		VkDevice device;
		VkRenderPass renderpass;
		uint32_t width;
		uint32_t height;
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
		VkDevice m_device;

		VkFramebuffer m_framebuffer;
	};
}