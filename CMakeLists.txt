cmake_minimum_required(VERSION 3.0.0)
project(PixelDancer-Vulkan VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(PixelDancer-Vulkan main.cpp Source/Instance.cpp)

include_directories($ENV{VULKAN_SDK}/Include Include/)

target_link_libraries(PixelDancer-Vulkan $ENV{VULKAN_SDK}/Lib/vulkan-1.lib)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
