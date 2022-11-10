#pragma once

#include "vulkan/vulkan.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#define VK_CHECK(res) vk_check(res, __FILE__, __LINE__)

static void vk_check(VkResult result, const char* file, int line)
{
	if (result == VK_SUCCESS) return;

	printf("::PXDERROR:: [%s] %d\n", file, line);
}