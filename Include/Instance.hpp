#pragma once

#include "Common.hpp"
#include <vector>
#include <string>

namespace pxd
{
	class Instance
	{
	public:
        Instance() = default;
		~Instance();

		void AddExtension(const char* ext);
		VkInstance* GetpInstance();
		void CreateInstance();

	private:
		std::vector<const char*> instanceExtensions;
		VkInstance m_instance = nullptr;
	};
}