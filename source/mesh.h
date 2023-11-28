#pragma once

#include "buffer.h"

#include "glm/vec3.hpp"

#include <vector>

namespace pxdvk
{
	struct VertexInputDescription
	{
		std::vector<VkVertexInputBindingDescription> bindings;
		std::vector<VkVertexInputAttributeDescription> attributes;

		VkPipelineVertexInputStateCreateFlags flags = 0;
	};

	class Mesh
	{
	private:
		struct Vertex
		{
			glm::vec3 pos;
			glm::vec3 normal;
			glm::vec3 color;
		};

	public:
		void init( const char* filepath );
		void init( size_t total_vertices );
		void destroy();

		void upload(VmaAllocator allocator);

		void draw(VkCommandBuffer cmd);

		static VertexInputDescription get_input_desc();

		Vertex& operator[]( int index )
		{
			return m_vertices [ index ];
		}

	private:
		std::vector<Vertex> m_vertices;
		size_t m_vertices_size;
		
		Buffer m_vertex_buffer;
	};
}