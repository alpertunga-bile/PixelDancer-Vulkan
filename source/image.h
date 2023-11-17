#pragma once

#include "debug.h"

namespace pxdvk
{
	class ImageView
	{
	public:
		static bool create_image_view( VkDevice device, VkImage image, VkImageViewType view_type, VkFormat format, VkImageAspectFlags aspect_flags, VkImageView* image_view );
	};
}