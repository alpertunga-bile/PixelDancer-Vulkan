#pragma once

#include "debug.h"

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

	class Buffer
	{
	public:
		void init( PxdBufferInfo info );
		void destroy();

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
		VkBuffer m_buffer = VK_NULL_HANDLE;
		VmaAllocator m_allocator = VK_NULL_HANDLE;
		VmaAllocation m_allocation = VK_NULL_HANDLE;
	};
}