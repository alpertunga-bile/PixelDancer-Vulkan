#include "swapchain.h"

#include "vulkan/vulkan.hpp"

namespace pxdvk
{
	struct SWDetails
	{
		VkSurfaceCapabilitiesKHR capabilities = {};
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> present_modes;
	};

	void Swapchain::init( PxdSwapchainInfo info )
	{
		m_device = info.device;

		create_swapchain( info );
		create_swapchain_images();
	}

	void Swapchain::destroy()
	{
		for ( size_t i = 0; i < m_swapchain_image_count; i++ )
		{
			vkDestroyImageView( m_device, m_swapchain_image_views [ i ], nullptr );
			m_framebuffers[i].destroy();
		}

		vkDestroySwapchainKHR( m_device, m_swapchain, nullptr );
	}

	void Swapchain::create_framebuffers(PxdFramebufferInfo fbi)
	{
		m_framebuffers.resize(m_swapchain_image_count);

		for (int i = 0; i < m_swapchain_image_count; i++)
		{
			fbi.image_views.clear();
			fbi.image_views.push_back(m_swapchain_image_views[i]);

			m_framebuffers[i].init(fbi);
		}
	}

	VkSurfaceFormatKHR Swapchain::choose_surface_format( VkSurfaceFormatKHR desired_surface_format, std::vector<VkSurfaceFormatKHR>& formats )
	{
		for ( int i = 0; i < formats.size(); i++ )
		{
			if ( formats [ i ].format == desired_surface_format.format &&
				formats [ i ].colorSpace == desired_surface_format.colorSpace )
			{
				return desired_surface_format;
			}
		}

		return formats[0];
	}

	VkPresentModeKHR Swapchain::choose_present_mode( std::vector<VkPresentModeKHR>& present_modes )
	{
		for ( VkPresentModeKHR present_mode : present_modes )
		{
			if ( present_mode == VK_PRESENT_MODE_MAILBOX_KHR )
			{
				return present_mode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	uint32_t Swapchain::choose_swap_image_count( VkSurfaceCapabilitiesKHR& caps )
	{
		uint32_t image_count = caps.minImageCount + 1;

		bool image_count_exceeded = caps.maxImageCount && image_count > caps.maxImageCount;

		return image_count_exceeded ? caps.maxImageCount : image_count;
	}

	SWDetails Swapchain::get_details( VkPhysicalDevice physical_device, VkSurfaceKHR surface )
	{
		SWDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR( physical_device, surface, &details.capabilities );

		uint32_t format_count;
		vkGetPhysicalDeviceSurfaceFormatsKHR( physical_device, surface, &format_count, nullptr );

		if ( format_count )
		{
			details.formats.resize( format_count );
			vkGetPhysicalDeviceSurfaceFormatsKHR( physical_device, surface, &format_count, details.formats.data() );
		}

		uint32_t present_mode_count;
		vkGetPhysicalDeviceSurfacePresentModesKHR( physical_device, surface, &present_mode_count, nullptr );

		if ( present_mode_count )
		{
			details.present_modes.resize( present_mode_count );
			vkGetPhysicalDeviceSurfacePresentModesKHR( physical_device, surface, &present_mode_count, details.present_modes.data() );
		}

		return details;
	}
	
	void Swapchain::create_swapchain( PxdSwapchainInfo& info )
	{
		SWDetails details = get_details( info.physicalDevice, info.surface );
		m_surface_format = choose_surface_format( info.surface_format, details.formats );
		m_present_mode = choose_present_mode( details.present_modes );
		m_swapchain_image_count = choose_swap_image_count( details.capabilities );

		VkSwapchainCreateInfoKHR sci = {};
		sci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		sci.pNext = nullptr;
		sci.flags = 0;
		sci.surface = info.surface;
		sci.minImageCount = m_swapchain_image_count;
		sci.imageFormat = m_surface_format.format;
		sci.imageColorSpace = m_surface_format.colorSpace;
		sci.imageExtent = { .width = info.width, .height = info.height };
		sci.imageArrayLayers = 1;
		sci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
			VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		sci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		sci.queueFamilyIndexCount = 1;
		sci.pQueueFamilyIndices = &info.graphics_family;
		sci.preTransform = details.capabilities.currentTransform;
		sci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		sci.presentMode = m_present_mode;
		sci.clipped = VK_TRUE;
		sci.oldSwapchain = VK_NULL_HANDLE;

		VK_CHECK(
			vkCreateSwapchainKHR( info.device, &sci, nullptr, &m_swapchain )
		);
	}
	
	void Swapchain::create_swapchain_images()
	{
		VK_CHECK(
			vkGetSwapchainImagesKHR( m_device, m_swapchain, &m_swapchain_image_count, nullptr )
		);


		m_swapchain_images.resize( m_swapchain_image_count );
		m_swapchain_image_views.resize( m_swapchain_image_count );

		VK_CHECK(
			vkGetSwapchainImagesKHR( m_device, m_swapchain, &m_swapchain_image_count, m_swapchain_images.data() )
		);

		for ( unsigned int i = 0; i < m_swapchain_image_count; i++ )
		{
			if ( !ImageView::create_image_view(
				m_device,
				m_swapchain_images [ i ],
				VK_IMAGE_VIEW_TYPE_2D,
				VK_FORMAT_B8G8R8A8_UNORM,
				VK_IMAGE_ASPECT_COLOR_BIT,
				&m_swapchain_image_views [ i ]
			) )
			{
				LOG_ERROR( "Swapchain image view cannot created" );
			}
		}
	}
}
