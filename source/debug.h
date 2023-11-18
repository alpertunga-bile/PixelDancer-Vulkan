#pragma once

#include "vulkan/vulkan.h"

#include <stdio.h>
#include <format>

inline void log_info( const char* msg )
{
	printf( "[    INFO] /_\\ %s\n", msg );
}

inline void log_success( const char* msg )
{
	printf( std::format("\033[0;32m[ SUCCESS] /_\\ {} \033[0m\n", msg).c_str() );
}

inline std::string get_output_string( const char* filename, int line )
{
	return std::format( "{} | {}", filename, line );
}

inline void log_warning( const char* msg, const char* file, int line )
{
	printf( std::format( "\033[0;33m[ WARNING] /_\\ {} /_\\ {} \033[0m\n", msg, get_output_string(file, line).c_str() ).c_str() );
}

inline void log_error( const char* msg, const char* file, int line )
{
	printf( std::format( "\033[0;31m[  FAILED] /_\\ {} /_\\ {} \033[0m\n", msg, get_output_string( file, line ).c_str() ).c_str() );
	exit( EXIT_FAILURE );
}

#if defined (_DEBUG)
#define LOG_INFO(msg) log_info(msg)
#define LOG_SUCCESS(msg) log_success(msg);
#define LOG_WARNING(msg) log_warning(msg, __FILE__, __LINE__);
#define LOG_ERROR(msg) log_error(msg, __FILE__, __LINE__);
#elif defined (_NDEBUG)
#define LOG_INFO(msg)
#define LOG_SUCCESS(msg)
#define LOG_WARNING(msg)
#define LOG_ERROR(msg)
#endif

inline void vk_check( VkResult result, const char* file, int line )
{
	if ( result == VK_SUCCESS )
	{
#if defined (_DEBUG)
		// LOG_SUCCESS( get_output_string(file, line).c_str() );
#endif
		return;
	}

	LOG_ERROR( get_output_string( file, line ).c_str() );
}

#define VK_CHECK(res) vk_check(res, __FILE__, __LINE__)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Debug Messenger & Report Callback

static VKAPI_ATTR VkBool32 VKAPI_CALL
VulkanDebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagsEXT type,
	const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
	void* user_data
)
{
	switch ( severity )
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		LOG_INFO( callback_data->pMessage );
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		LOG_WARNING( callback_data->pMessage );
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		LOG_ERROR( callback_data->pMessage );
		break;
	default:
		printf( "Validation Layer : %s\n", callback_data->pMessage );
		break;
	}

	return VK_FALSE;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
VulkanDebugReportCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT object_type,
	uint64_t object,
	size_t location,
	int32_t message_code,
	const char* pLayerPrefix,
	const char* pMessage,
	void* user_data
)
{
	if ( flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT )
	{
		return VK_FALSE;
	}

	printf( "Debug Report Callback => [%s] : %s\n", pLayerPrefix, pMessage );

	return VK_FALSE;
}