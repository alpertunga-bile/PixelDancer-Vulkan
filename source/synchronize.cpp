#include "synchronize.h"

namespace pxdvk
{
	void Semaphore::init( VkDevice device, VkSemaphoreCreateFlags flags )
	{
		m_device = device;

		VkSemaphoreCreateInfo sci = {};
		sci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		sci.pNext = nullptr;
		sci.flags = flags;

		VK_CHECK(
			vkCreateSemaphore( m_device, &sci, nullptr, &m_semaphore )
		);
	}
	
	void Semaphore::destroy()
	{
		vkDestroySemaphore( m_device, m_semaphore, nullptr );
	}

	void Fence::init( VkDevice device, VkFenceCreateFlags flags )
	{
		m_device = device;

		VkFenceCreateInfo fci = {};
		fci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fci.pNext = nullptr;
		fci.flags = flags;

		VK_CHECK(
			vkCreateFence( m_device, &fci, nullptr, &m_fence )
		);
	}

	void Fence::destroy()
	{
		vkDestroyFence( m_device, m_fence, nullptr );
	}

	void Fence::wait( VkBool32 wait_all, uint64_t wait_for_seconds )
	{
		uint64_t timeout = wait_for_seconds * 1000000000;

		VK_CHECK(
			vkWaitForFences( m_device, 1, &m_fence, wait_all, timeout )
		);
	}
	
	void Fence::reset()
	{
		VK_CHECK(
			vkResetFences( m_device, 1, &m_fence )
		);
	}
}
