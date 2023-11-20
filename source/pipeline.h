#pragma once

#include "debug.h"
#include <unordered_map>

namespace pxdvk
{
	class PipelineLayout
	{
	public:
		void init(VkDevice device, std::vector<VkDescriptorSetLayout>& descriptor_set_layouts, std::vector<VkPushConstantRange>& push_constant_ranges);
		void destroy();

		inline VkPipelineLayout get()
		{
			return m_pipeline_layout;
		}

	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
	};

	class Pipeline
	{
	public:
		void init(VkDevice device, VkRenderPass renderpass, VkPipelineLayout pipeline_layout);
		void destroy();

		inline VkPipeline get()
		{
			return m_pipeline;
		}

		void add_shader( VkShaderStageFlagBits stage, VkShaderModule module );
		void create_vertex_input( std::vector<VkVertexInputBindingDescription>& input_binding, std::vector<VkVertexInputAttributeDescription>& input_attributes );
		void create_assembly_info( VkPrimitiveTopology topology );
		void create_rasterization( VkPolygonMode polygon_mode );
		void create_multisampling_state();
		void create_color_blend( std::vector<VkPipelineColorBlendAttachmentState>& m_color_blend_attachments );
		void create_viewport_state();

		void set_viewport( float x, float y, float width, float height, float min_depth, float max_depth );
		void set_scissor( int32_t x, int32_t y, uint32_t width, uint32_t height );

	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VkPipeline m_pipeline = VK_NULL_HANDLE;

		VkViewport m_viewport;
		VkRect2D m_scissor;

		std::vector<VkPipelineShaderStageCreateInfo> m_shader_stages;
		VkPipelineVertexInputStateCreateInfo m_vertex_input_info = {};
		VkPipelineInputAssemblyStateCreateInfo m_input_assembly = {};
		VkPipelineRasterizationStateCreateInfo m_rasterizer = {};
		VkPipelineViewportStateCreateInfo m_viewport_state = {};
		VkPipelineColorBlendStateCreateInfo m_color_blend_state = {};
		VkPipelineMultisampleStateCreateInfo m_multisampling = {};
	};
}