#include "image.h"

namespace pxdvk
{
	bool ImageView::create_image_view( VkDevice device, VkImage image, VkImageViewType view_type, VkFormat format, VkImageAspectFlags aspect_flags, VkImageView* image_view )
	{
		VkImageViewCreateInfo ivci = {};
		ivci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ivci.pNext = nullptr;
		ivci.flags = 0;
		ivci.image = image;
		ivci.viewType = view_type;
		ivci.format = format;
		ivci.subresourceRange = {
			.aspectMask = aspect_flags,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		};

		VK_CHECK(
			vkCreateImageView( device, &ivci, nullptr, image_view )
		);

		return true;
	}
}
