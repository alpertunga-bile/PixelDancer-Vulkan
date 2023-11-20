#pragma once

#include "debug.h"

namespace pxdvk
{
	class Renderpass
	{
	public:
		void init( VkDevice device, VkFormat image_format, VkImageLayout color_attachment_image_layout );
		void destroy();

		VkRenderPass get()
		{
			return m_renderpass;
		}

	private:
		VkDevice m_device = VK_NULL_HANDLE;

		VkRenderPass m_renderpass = VK_NULL_HANDLE;
	};
}