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
}