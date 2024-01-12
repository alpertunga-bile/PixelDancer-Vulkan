#pragma once

#include "debug.h"

#include "vma/vk_mem_alloc.h"

namespace pxdvk
{
	class ImageView
	{
	public:
		static bool create_image_view( VkDevice device, VkImage image, VkImageViewType view_type, VkFormat format, VkImageAspectFlags aspect_flags, VkImageView* image_view );
	};

	class Image
	{
		VmaAllocator m_allocator;

		VkImage m_image;
		VmaAllocation m_allocation;
	};
}