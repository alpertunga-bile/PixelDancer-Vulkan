#pragma once

#include "debug.h"

#include "commandpool.h"
#include "nexus.h"

#include "vma/vk_mem_alloc.h"

namespace pxdvk
{
	struct PxdBufferInfo
	{
		VmaAllocator allocator;
		VkDeviceSize size;
		VkBufferUsageFlagBits buffer_usage;
		VmaMemoryUsage memory_usage;
	};

	struct PxdCopyBufferInfo
	{
		VkDevice device;
		CommandPool commandpool;
		Queue queue;
		VkBuffer dst_buffer;
		VkDeviceSize size;
	};

	class Buffer
	{
	public:
		void init( PxdBufferInfo info );
		void destroy();

		void copy_to( PxdCopyBufferInfo copy_info );

		VkBuffer get()
		{
			return m_buffer;
		}

		VkBuffer* get_ptr()
		{
			return &m_buffer;
		}

		VmaAllocator get_allocator()
		{
			return m_allocator;
		}

		VmaAllocation get_allocation()
		{
			return m_allocation;
		}

	private:
		void begin_single_time_commands( VkDevice device, CommandPool commandpool );
		void end_single_time_commands( VkDevice device, CommandPool commandpool, Queue queue );

	private:
		VkBuffer m_buffer = VK_NULL_HANDLE;
		VmaAllocator m_allocator = VK_NULL_HANDLE;
		VmaAllocation m_allocation = VK_NULL_HANDLE;
	};
}