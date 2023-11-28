#pragma once

#include "buffer.h"
#include "pipeline.h"

#include "glm/gtx/transform.hpp"

#include <vector>

namespace pxdvk
{
	struct VertexInputDescription
	{
		std::vector<VkVertexInputBindingDescription> bindings;
		std::vector<VkVertexInputAttributeDescription> attributes;

		VkPipelineVertexInputStateCreateFlags flags = 0;
	};

	struct MeshPushConstants
	{
		glm::mat4 data;
		glm::mat4 render_matrix;
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

		void draw(VkCommandBuffer cmd, MeshPushConstants& push_constant, VkPipelineLayout pipe_layout);

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