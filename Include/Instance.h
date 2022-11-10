#pragma once

#include "Common.h"
#include <vector>
#include <string>

namespace pxd
{
	class Instance
	{
	public:
		~Instance();

		void AddValidationLayer(const char* valLayer);
		void AddExtension(const char* ext);

		VkInstance* GetpInstance();
		VkDebugUtilsMessengerEXT* GetpDebugMessenger();

	private:
		void CreateInstance();
		void CreateDebugMessenger();
		bool CheckValidationLayer();

	private:
		std::vector<const char*> validationLayers;
		std::vector<const char*> instanceExtensions;
		VkInstance m_instance;
		VkDebugUtilsMessengerEXT m_debugMessenger;
	};
}