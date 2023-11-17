#include "renderpass.h"

namespace pxdvk
{
	void Renderpass::init( VkDevice device, VkFormat image_format, VkImageLayout color_attachment_image_layout )
	{
		m_device = device;

		VkAttachmentDescription color_attachment = {};
		color_attachment.format = image_format;
		color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference color_attachment_ref = {};
		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout = color_attachment_image_layout;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment_ref;

		VkRenderPassCreateInfo rpci = {};
		rpci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		rpci.attachmentCount = 1;
		rpci.pAttachments = &color_attachment;
		rpci.subpassCount = 1;
		rpci.pSubpasses = &subpass;

		VK_CHECK(
			vkCreateRenderPass( m_device, &rpci, nullptr, &m_renderpass )
		);
	}

	void Renderpass::destroy()
	{
		vkDestroyRenderPass( m_device, m_renderpass, nullptr );
	}
}
