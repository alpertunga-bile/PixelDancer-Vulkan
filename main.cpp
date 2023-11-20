#include <iostream>

#include "source/application.h"

int main()
{
	uint32_t width = 1280;
	uint32_t height = 720;

	glfwInit();

	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
	glfwWindowHint( GLFW_RESIZABLE, 0 );

	GLFWwindow* window = glfwCreateWindow( width, height, "PXD Vulkan", nullptr, nullptr );

	pxdvk::Application app;
	app.initialize( window );

	int frame_number = 0;

	// ---------------------------------------------------------------------------------------------------------------------
	// Window

	while ( !glfwWindowShouldClose(window) )
	{
		app.render( frame_number );

		glfwPollEvents();

		frame_number++;
	}

	// ---------------------------------------------------------------------------------------------------------------------
	// Destroy

	app.destroy();
	glfwTerminate();
	
    return 0;
}
