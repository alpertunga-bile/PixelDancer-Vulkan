#include "commandpool.h"

namespace pxdvk
{
	void CommandPool::init( PxdCommandPoolInfo info )
	{
		m_device = info.device;

		VkCommandPoolCreateInfo cpci = {};
		cpci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cpci.pNext = nullptr;
		cpci.queueFamilyIndex = info.queue_family;
		cpci.flags = info.flags;

		VK_CHECK(
			vkCreateCommandPool( m_device, &cpci, nullptr, &m_commandpool )
		);
	}

	void CommandPool::allocate( std::vector<PxdCommandBufferInfo>& buffer_infos )
	{
		for ( PxdCommandBufferInfo buffer_info : buffer_infos )
		{
			CommandBuffer cmd;
			cmd.init( m_device, m_commandpool, buffer_info.level );

			m_commandbuffer_map.insert( { buffer_info.name, cmd } );
		}
	}
	
	void CommandPool::destroy()
	{
		vkDestroyCommandPool( m_device, m_commandpool, nullptr );
	}
}
