#pragma once

#include "commandbuffer.h"

#include <vector>
#include <unordered_map>

namespace pxdvk
{
	struct PxdCommandPoolInfo
	{
		VkDevice device = VK_NULL_HANDLE;
		uint32_t queue_family = 0;
		VkCommandPoolCreateFlags flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	};

	struct PxdCommandBufferInfo
	{
		std::string name = "main";
		VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	};

	class CommandPool
	{
	public:
		void init(PxdCommandPoolInfo info);

		void allocate( std::vector<PxdCommandBufferInfo>& buffer_infos );

		void destroy();

		CommandBuffer operator[]( std::string buffer_name )
		{
			return m_commandbuffer_map [ buffer_name ];
		}

		CommandBuffer operator[]( int index )
		{
			if ( index >= m_commandbuffer_map.size() )
			{
				return CommandBuffer();
			}

			auto it = m_commandbuffer_map.begin();
			std::advance( it, index );
			return it->second;
		}

	private:
		VkCommandPool get() const
		{
			return m_commandpool;
		}

		VkCommandPool* get_ptr()
		{
			return &m_commandpool;
		}

	private:
		VkDevice m_device;
		VkCommandPool m_commandpool;

		std::unordered_map<std::string, CommandBuffer> m_commandbuffer_map;
	};
}