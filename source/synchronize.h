#pragma once

#include "debug.h"

namespace pxdvk
{
	class Semaphore
	{
	public:
		void init( VkDevice device, VkSemaphoreCreateFlags flags = 0 );
		void destroy();

		VkSemaphore* get_ptr()
		{
			return &m_semaphore;
		}

		VkSemaphore get()
		{
			return m_semaphore;
		}

	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VkSemaphore m_semaphore = VK_NULL_HANDLE;
	};

	class Fence
	{
	public:
		void init( VkDevice device, VkFenceCreateFlags flags );
		void destroy();

		void wait( VkBool32 wait_all = VK_TRUE, uint64_t wait_for_seconds = 5 );
		void reset();

		VkFence* get_ptr()
		{
			return &m_fence;
		}

		VkFence get()
		{
			return m_fence;
		}

	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VkFence m_fence = VK_NULL_HANDLE;
	};
}