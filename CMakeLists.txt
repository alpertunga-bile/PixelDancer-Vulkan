cmake_minimum_required(VERSION 3.0.0)
project(PixelDancer-Vulkan VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 20)

FILE(GLOB SRCFILES source/*.cpp)

include_directories(
    $ENV{VULKAN_SDK}/Include
    third-party/include/
)

link_directories($ENV{VULKAN_SDK}/Lib/ third-party/lib/${CONFIGURATION})

add_executable(${PROJECT_NAME} main.cpp ${SRCFILES})

target_link_libraries(${PROJECT_NAME} vulkan-1 glfw3)

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/third-party/dll/$<CONFIGURATION>"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
