#include "nexus.h"

namespace pxdvk
{
	void Nexus::init( PxdNexusCreateInfo create_info )
	{
		init_instance( create_info.app_name, create_info.api_version );
		init_debugs();
		init_physical_device( create_info.physical_device_selector );

		for ( PxdQueueInfo queue_info : create_info.queue_infos )
		{
			add_queue_info( queue_info );
		}
		
		init_device( create_info.enable_raytracing );
		init_queues();
	}

	void Nexus::destroy()
	{
		vkDestroyDevice( m_device, nullptr );

		vkDestroySurfaceKHR( m_instance, m_surface, nullptr );

#if defined (_DEBUG)
		PFN_vkDestroyDebugUtilsMessengerEXT destroy_msg_func = ( PFN_vkDestroyDebugUtilsMessengerEXT ) vkGetInstanceProcAddr( m_instance, "vkDestroyDebugUtilsMessengerEXT" );

		if ( destroy_msg_func )
		{
			destroy_msg_func( m_instance, m_messenger, nullptr );
		}
		else
		{
			LOG_ERROR( "Failed to destroy debug utils messenger" );
		}

		PFN_vkDestroyDebugReportCallbackEXT destroy_report_callback_func = ( PFN_vkDestroyDebugReportCallbackEXT ) vkGetInstanceProcAddr( m_instance, "vkDestroyDebugReportCallbackEXT" );

		if ( destroy_report_callback_func )
		{
			destroy_report_callback_func( m_instance, m_report_callback, nullptr );
		}
		else
		{
			LOG_ERROR( "Failed to destroy debug report callback" );
		}
#endif

		vkDestroyInstance( m_instance, nullptr );
	}

	void Nexus::add_instance_extension( const char* ext )
	{
		auto it = std::find( m_instance_extensions.begin(), m_instance_extensions.end(), ext );

		if ( it == m_instance_extensions.end() )
		{
			LOG_INFO( std::format( "{} instance extension is already included", ext ).c_str() );
			return;
		}

		m_instance_extensions.push_back( ext );
	}

	void Nexus::remove_instance_extension( const char* ext )
	{
		auto it = std::find( m_instance_extensions.begin(), m_instance_extensions.end(), ext );

		if ( it == m_instance_extensions.end() )
		{
			LOG_INFO( std::format( "{} instance extension is not found", ext ).c_str() );
			return;
		}

		m_instance_extensions.erase( it );
	}

	uint32_t Nexus::get_queue_family( VkQueueFlags queue_flag, VkPhysicalDevice physical_device )
	{
		uint32_t family_count;
		vkGetPhysicalDeviceQueueFamilyProperties(
			physical_device, &family_count, nullptr
		);
		std::vector<VkQueueFamilyProperties> families( family_count );
		vkGetPhysicalDeviceQueueFamilyProperties(
			physical_device, &family_count, families.data()
		);

		for ( uint32_t i = 0; i < family_count; i++ )
		{
			if ( families [ i ].queueCount &&
				( families [ i ].queueFlags & queue_flag )
				)
			{
				return i;
			}
		}

		return 0;
	}

	VkQueue Nexus::get_queue( std::string queue_name )
	{
		auto it = m_queue_map.find( queue_name );

		if ( it == m_queue_map.end() )
		{
			return VkQueue();
		}

		return it->second.queue;
	}

	Queue Nexus::get_queue_class(std::string queue_name)
	{
		auto it = m_queue_map.find(queue_name);

		if (it == m_queue_map.end())
		{
			return Queue();
		}

		return it->second;
	}

	void Nexus::init_instance(const char* app_name, uint32_t api_version)
	{
		VkApplicationInfo app_info = {};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.pNext = nullptr;
		app_info.pApplicationName = app_name;
		app_info.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
		app_info.pEngineName = "Vulkan Engine";
		app_info.engineVersion = VK_MAKE_VERSION( 1, 0, 0 );
		app_info.apiVersion = api_version;

		VkInstanceCreateInfo ci = {};
		ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		ci.pNext = nullptr;
		ci.flags = 0;
		ci.pApplicationInfo = &app_info;
		
#if defined (_DEBUG)
		ci.enabledLayerCount = static_cast< uint32_t >( m_validation_layers.size() );
		ci.ppEnabledLayerNames = m_validation_layers.data();
#endif

		ci.enabledExtensionCount = static_cast< uint32_t >( m_instance_extensions.size() );
		ci.ppEnabledExtensionNames = m_instance_extensions.data();

		VK_CHECK(
			vkCreateInstance( &ci, nullptr, &m_instance )
		);
	}
	
	void Nexus::init_debugs()
	{
		VkDebugUtilsMessengerCreateInfoEXT mci = {};
		mci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		mci.pNext = nullptr;
		mci.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		mci.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		mci.pfnUserCallback = &VulkanDebugCallback;
		mci.pUserData = nullptr;

		PFN_vkCreateDebugUtilsMessengerEXT msg_func = ( PFN_vkCreateDebugUtilsMessengerEXT ) vkGetInstanceProcAddr( m_instance, "vkCreateDebugUtilsMessengerEXT" );

		if ( msg_func )
		{
			msg_func( m_instance, &mci, nullptr, &m_messenger );
			LOG_SUCCESS( "Debug Utils Messenger is created" );
		}
		else
		{
			LOG_ERROR( "Failed to create debug utils messenger" );
		}

		VkDebugReportCallbackCreateInfoEXT rpci = {};
		rpci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		rpci.pNext = nullptr;
		rpci.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT |
			VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
			VK_DEBUG_REPORT_ERROR_BIT_EXT |
			VK_DEBUG_REPORT_DEBUG_BIT_EXT;
		rpci.pfnCallback = &VulkanDebugReportCallback;
		rpci.pUserData = nullptr;

		PFN_vkCreateDebugReportCallbackEXT report_callback_func = ( PFN_vkCreateDebugReportCallbackEXT ) vkGetInstanceProcAddr( m_instance, "vkCreateDebugReportCallbackEXT" );

		if ( report_callback_func )
		{
			report_callback_func( m_instance, &rpci, nullptr, &m_report_callback );
			LOG_SUCCESS( "Debug report callback is created" );
		}
		else
		{
			LOG_ERROR( "Failed to create debug report callback" );
		}
	}
	
	void Nexus::init_physical_device( std::function<bool( VkPhysicalDevice )> selector )
	{
		uint32_t device_count = 0;
		vkEnumeratePhysicalDevices( m_instance, &device_count, nullptr );

		if ( !device_count )
		{
			LOG_ERROR( "Can't enumerate physical devices" );
		}

		std::vector<VkPhysicalDevice> physical_devices(device_count);
		vkEnumeratePhysicalDevices( m_instance, &device_count, physical_devices.data() );

		for ( VkPhysicalDevice device : physical_devices )
		{
			if ( selector( device ) )
			{
				m_physical_device = device;
				break;
			}
		}

		if ( !m_physical_device )
		{
			LOG_ERROR( "Can't find physical device" );
		}

		LOG_SUCCESS( "Physical device is created" );
	}
	
	void Nexus::add_queue_info( PxdQueueInfo info )
	{
		uint32_t family_count;
		vkGetPhysicalDeviceQueueFamilyProperties(
			m_physical_device, &family_count, nullptr
		);
		std::vector<VkQueueFamilyProperties> families( family_count );
		vkGetPhysicalDeviceQueueFamilyProperties(
			m_physical_device, &family_count, families.data()
		);

		int queue_index = 0;

		for ( uint32_t i = 0; i < family_count; i++ )
		{
			if ( families [ i ].queueCount &&
				( families [ i ].queueFlags & info.flag )
				)
			{
				queue_index = i;
			}
		}

		Queue pxd_queue;
		pxd_queue.flag = info.flag;
		pxd_queue.queue = VK_NULL_HANDLE;
		pxd_queue.family_index = queue_index;
		pxd_queue.queue_index = info.queue_index;

		m_queue_map.insert( { info.name, pxd_queue } );

		VkDeviceQueueCreateInfo qci = {};
		qci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		qci.pNext = nullptr;
		qci.flags = 0;
		qci.queueFamilyIndex = queue_index;
		qci.queueCount = 1;
		qci.pQueuePriorities = &info.priority;

		m_queue_create_infos.push_back( qci );
	}

	void Nexus::init_device(bool enable_raytracing)
	{
		VkPhysicalDeviceFeatures2 device_features2 {};
		
		if ( enable_raytracing )
		{
			m_device_extensions.push_back( VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME );
			m_device_extensions.push_back( VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME );
			m_device_extensions.push_back( VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME );
			m_device_extensions.push_back( VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME );
			m_device_extensions.push_back( VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME );
			m_device_extensions.push_back( VK_KHR_SPIRV_1_4_EXTENSION_NAME );
			m_device_extensions.push_back( VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME );

			VkPhysicalDeviceBufferDeviceAddressFeatures address_features = {};
			address_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
			address_features.bufferDeviceAddress = VK_TRUE;

			VkPhysicalDeviceRayTracingPipelineFeaturesKHR pipeline_features = {};
			pipeline_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
			pipeline_features.rayTracingPipeline = VK_TRUE;
			pipeline_features.pNext = &address_features;

			VkPhysicalDeviceAccelerationStructureFeaturesKHR accel_features = {};
			accel_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
			accel_features.accelerationStructure = VK_TRUE;
			accel_features.pNext = &pipeline_features;

			device_features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
			device_features2.pNext = &accel_features;

			vkGetPhysicalDeviceFeatures2( m_physical_device, &device_features2 );
		}

		VkDeviceCreateInfo ci = {};
		ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		ci.pNext = nullptr;
		ci.flags = 0;
		ci.queueCreateInfoCount = static_cast< uint32_t >( m_queue_create_infos.size() );
		ci.pQueueCreateInfos = m_queue_create_infos.data();
		ci.enabledLayerCount = 0;
		ci.ppEnabledLayerNames = nullptr;
		ci.enabledExtensionCount = static_cast< uint32_t >( m_device_extensions.size() );
		ci.ppEnabledExtensionNames = m_device_extensions.data();

		if ( enable_raytracing )
		{
			ci.pNext = &device_features2;
			ci.pEnabledFeatures = nullptr;
		}
		else
		{
			VkPhysicalDeviceFeatures features;
			vkGetPhysicalDeviceFeatures( m_physical_device, &features );
			
			ci.pNext = nullptr;
			ci.pEnabledFeatures = &features;
		}

		VK_CHECK(
			vkCreateDevice( m_physical_device, &ci, nullptr, &m_device )
		);
	}

	void Nexus::init_queues()
	{
		for ( auto& p : m_queue_map )
		{
			vkGetDeviceQueue( 
				m_device, 
				p.second.family_index, 
				p.second.queue_index,
				&p.second.queue 
			);
		}
	}

	void Nexus::print_physical_device_info()
	{
		VkPhysicalDeviceProperties device_properties;
		vkGetPhysicalDeviceProperties( m_physical_device, &device_properties );

		printf( "@@@@@@@@@@@@@@@@@ Selected GPU @@@@@@@@@@@@@@@@@\n" );
		printf( "-- %s ----------------------------------------------\n", device_properties.deviceName );
		printf( "Vendor ID : %d\n", device_properties.vendorID );
		printf( "Device ID : %d\n", device_properties.deviceID );
		
		switch ( device_properties.deviceType )
		{
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			printf( "Device Type : Integrated GPU\n" );
			break;
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			printf( "Device Type : Discrete GPU\n" );
			break;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			printf( "Device Type : Virtual GPU\n" );
			break;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			printf( "Device Type : CPU\n" );
			break;
		default:
			printf( "Device Type : Unknown\n" );
			break;
		}

		printf( "API Version : %d\n", device_properties.apiVersion );
		printf( "Driver Version : %d\n", device_properties.driverVersion );		
	}
}
