#pragma once

#include "image.h"
#include "framebuffer.h"

#include <vector>

namespace pxdvk
{
	struct SWDetails;

	struct PxdSwapchainInfo
	{
		VkSurfaceFormatKHR surface_format = { .format = VK_FORMAT_B8G8R8A8_UNORM, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		VkDevice device = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		uint32_t graphics_family = 0;
		uint32_t width = 1280;
		uint32_t height = 720;
	};

	class Swapchain
	{
	public:
		void init( PxdSwapchainInfo info );

		void destroy();

		void create_framebuffers(PxdFramebufferInfo fbi);

		operator VkSwapchainKHR() const
		{
			return m_swapchain;
		}

		VkSwapchainKHR* get_ptr()
		{
			return &m_swapchain;
		}

		VkFormat get_image_format() const
		{
			return m_surface_format.format;
		}

		uint32_t get_image_count() const
		{
			return m_swapchain_image_count;
		}

		VkImageView get_img_view( int index )
		{
			if ( index >= m_swapchain_image_count )
			{
				return VkImageView();
			}

			return m_swapchain_image_views [ index ];
		}

		VkImage get_img( int index )
		{
			if ( index >= m_swapchain_image_count )
			{
				return VkImage();
			}

			return m_swapchain_images [ index ];
		}

		VkFramebuffer get_framebuffer(int index)
		{
			if (index >= m_swapchain_image_count)
			{
				return VkFramebuffer();
			}

			return m_framebuffers[index].get();
		}

	private:
		VkSurfaceFormatKHR choose_surface_format( VkSurfaceFormatKHR desired_surface_format, std::vector<VkSurfaceFormatKHR>& formats );
		VkPresentModeKHR choose_present_mode( std::vector<VkPresentModeKHR>& present_modes );
		uint32_t choose_swap_image_count( VkSurfaceCapabilitiesKHR& caps );
		SWDetails get_details( VkPhysicalDevice physical_device, VkSurfaceKHR surface );
		void create_swapchain( PxdSwapchainInfo& info );
		void create_swapchain_images();

	private:
		VkDevice m_device = VK_NULL_HANDLE;

		VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
		std::vector<VkImage> m_swapchain_images;
		std::vector<VkImageView> m_swapchain_image_views;
		
		VkSurfaceFormatKHR m_surface_format;
		VkPresentModeKHR m_present_mode;
		uint32_t m_swapchain_image_count;

		std::vector<Framebuffer> m_framebuffers;
	};
}