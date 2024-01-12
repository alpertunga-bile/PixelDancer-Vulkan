#include "buffer.h"

namespace pxdvk
{
	void Buffer::init( PxdBufferInfo info )
	{
		m_allocator = info.allocator;

		VkBufferCreateInfo bci = {};
		bci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bci.size = info.size;
		bci.usage = info.buffer_usage;

		VmaAllocationCreateInfo vaci = {};
		vaci.usage = info.memory_usage;

		VK_CHECK(
			vmaCreateBuffer(
				m_allocator,
				&bci,
				&vaci,
				&m_buffer,
				&m_allocation,
				nullptr
			)
		);
	}
	
	void Buffer::destroy()
	{
		vmaDestroyBuffer( m_allocator, m_buffer, m_allocation );
	}
	
	void Buffer::copy_to( PxdCopyBufferInfo copy_info )
	{
		begin_single_time_commands( copy_info.device, copy_info.commandpool );

		VkBufferCopy copy_param = {};
		copy_param.srcOffset = 0;
		copy_param.dstOffset = 0;
		copy_param.size = copy_info.size;

		vkCmdCopyBuffer(
			copy_info.commandpool [ "copy_buffer" ].get(),
			m_buffer,
			copy_info.dst_buffer,
			1,
			&copy_param
		);

		end_single_time_commands( copy_info.device, copy_info.commandpool, copy_info.queue );
	}
	
	void Buffer::begin_single_time_commands( VkDevice device, CommandPool commandpool )
	{
		std::vector<PxdCommandBufferInfo> cmdinfos;

		PxdCommandBufferInfo cbi = {};
		cbi.name = "copy_buffer";
		cbi.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		cmdinfos.push_back( cbi );

		commandpool.allocate( cmdinfos );

		commandpool [ "copy_buffer" ].begin_recording( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr );
	}
	
	void Buffer::end_single_time_commands( VkDevice device, CommandPool commandpool, Queue queue )
	{
		commandpool [ "copy_buffer" ].end_recording();

		PxdQueueSubmitInfo qsi = {};
		qsi.commandbuffers.push_back( commandpool [ "copy_buffer" ].get() );

		queue.submit( qsi );

		vkQueueWaitIdle( queue.queue );

		commandpool.free_commandbuffer( "copy_buffer" );
	}
}