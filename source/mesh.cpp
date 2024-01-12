#include "mesh.h"

namespace pxdvk
{
	void Mesh::init( const char* filepath )
	{
	}
	
	void Mesh::init( size_t total_vertices )
	{
		m_vertices.resize( total_vertices );
	}

	void Mesh::destroy()
	{
		m_vertex_buffer.destroy();
	}

	void Mesh::upload( VmaAllocator allocator )
	{
		m_vertices_size = m_vertices.size();

		PxdBufferInfo bi = {};
		bi.allocator = allocator;
		bi.buffer_usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bi.size = m_vertices.size() * sizeof( Vertex );
		bi.memory_usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		m_vertex_buffer.init( bi );

		void* data;

		vmaMapMemory( m_vertex_buffer.get_allocator(), m_vertex_buffer.get_allocation(), &data );

		memcpy( data, m_vertices.data(), m_vertices.size() * sizeof( Vertex ) );

		vmaUnmapMemory( m_vertex_buffer.get_allocator(), m_vertex_buffer.get_allocation() );
	
		m_vertices.clear();
	}

	void Mesh::draw( VkCommandBuffer cmd, MeshPushConstants& push_constant, VkPipelineLayout pipe_layout )
	{
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers( cmd, 0, 1, m_vertex_buffer.get_ptr(), &offset );

		vkCmdPushConstants( cmd, pipe_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof( MeshPushConstants ), &push_constant );

		vkCmdDraw( cmd, m_vertices_size, 1, 0, 0 );
	}

	VertexInputDescription Mesh::get_input_desc()
	{
		VertexInputDescription desc;

		VkVertexInputBindingDescription bind = {};
		bind.binding = 0;
		bind.stride = sizeof( Vertex );
		bind.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		desc.bindings.push_back( bind );

		VkVertexInputAttributeDescription pos_attr = {};
		pos_attr.binding = 0;
		pos_attr.location = 0;
		pos_attr.format = VK_FORMAT_R32G32B32_SFLOAT;
		pos_attr.offset = offsetof( Vertex, pos );

		desc.attributes.push_back( pos_attr );

		VkVertexInputAttributeDescription normal_attr = {};
		normal_attr.binding = 0;
		normal_attr.location = 1;
		normal_attr.format = VK_FORMAT_R32G32B32_SFLOAT;
		normal_attr.offset = offsetof( Vertex, normal );

		desc.attributes.push_back( normal_attr );

		VkVertexInputAttributeDescription color_attr = {};
		color_attr.binding = 0;
		color_attr.location = 2;
		color_attr.format = VK_FORMAT_R32G32B32_SFLOAT;
		color_attr.offset = offsetof( Vertex, color );

		desc.attributes.push_back( color_attr );

		return desc;
	}
}