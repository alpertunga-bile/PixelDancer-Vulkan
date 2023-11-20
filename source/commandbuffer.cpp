#include "commandbuffer.h"

namespace pxdvk
{
	void CommandBuffer::init( VkDevice device, VkCommandPool commandpool, VkCommandBufferLevel level )
	{
		VkCommandBufferAllocateInfo cbai = {};
		cbai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cbai.pNext = nullptr;

		cbai.commandPool = commandpool;
		cbai.commandBufferCount = 1;
		cbai.level = level;

		VK_CHECK(
			vkAllocateCommandBuffers( device, &cbai, &m_commandbuffer )
		);
	}

	void CommandBuffer::destroy()
	{
	}
	
	void CommandBuffer::reset()
	{
		VK_CHECK(
			vkResetCommandBuffer( m_commandbuffer, 0 )
		);
	}
	
	void CommandBuffer::begin_recording( VkCommandBufferUsageFlags usage_flags, VkCommandBufferInheritanceInfo* pInheritance )
	{
		VkCommandBufferBeginInfo cbbi = {};
		cbbi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cbbi.pNext = nullptr;
		cbbi.flags = usage_flags;
		cbbi.pInheritanceInfo = pInheritance;

		VK_CHECK(
			vkBeginCommandBuffer( m_commandbuffer, &cbbi )
		);
	}

	void CommandBuffer::end_recording()
	{
		vkEndCommandBuffer(m_commandbuffer);
	}

	void CommandBuffer::begin_renderpass(VkRenderPassBeginInfo* rpbi, VkSubpassContents subpass_contents)
	{
		vkCmdBeginRenderPass(
			m_commandbuffer,
			rpbi,
			subpass_contents
		);
	}

	void CommandBuffer::end_renderpass()
	{
		vkCmdEndRenderPass(m_commandbuffer);
	}
	
	void CommandBuffer::bind_pipeline( VkPipelineBindPoint bind_point, VkPipeline pipeline )
	{
		vkCmdBindPipeline( m_commandbuffer, bind_point, pipeline );
	}
	
	void CommandBuffer::draw( uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance )
	{
		vkCmdDraw( m_commandbuffer, vertex_count, instance_count, first_vertex, first_instance );
	}
}
