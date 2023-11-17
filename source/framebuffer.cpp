#include "framebuffer.h"

namespace pxdvk
{
	void Framebuffer::init( PxdFramebufferInfo info )
	{
		m_device = info.device;

		VkFramebufferCreateInfo fci = {};
		fci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fci.pNext = nullptr;

		fci.renderPass = info.renderpass;
		fci.width = info.width;
		fci.height = info.height;
		fci.layers = info.depth;

		fci.attachmentCount = static_cast< uint32_t >( info.image_views.size() );
		fci.pAttachments = info.image_views.data();

		VK_CHECK(
			vkCreateFramebuffer( m_device, &fci, nullptr, &m_framebuffer )
		);
	}

	void Framebuffer::destroy()
	{
		vkDestroyFramebuffer( m_device, m_framebuffer, nullptr );
	}
}
