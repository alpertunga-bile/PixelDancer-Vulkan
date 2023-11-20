#include "pipeline.h"

namespace pxdvk
{
	void Pipeline::init( VkDevice device, VkRenderPass renderpass, VkPipelineLayout pipeline_layout )
	{
		m_device = device;

		VkGraphicsPipelineCreateInfo pipeline_info = {};
		pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_info.pNext = nullptr;

		pipeline_info.stageCount = m_shader_stages.size();
		pipeline_info.pStages = m_shader_stages.data();
		pipeline_info.pVertexInputState = &m_vertex_input_info;
		pipeline_info.pInputAssemblyState = &m_input_assembly;
		pipeline_info.pViewportState = &m_viewport_state;
		pipeline_info.pRasterizationState = &m_rasterizer;
		pipeline_info.pMultisampleState = &m_multisampling;
		pipeline_info.pColorBlendState = &m_color_blend_state;
		pipeline_info.layout = pipeline_layout;
		pipeline_info.renderPass = renderpass;
		pipeline_info.subpass = 0;
		pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

		VK_CHECK(
			vkCreateGraphicsPipelines(
				m_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_pipeline
			)
		);
	}

	void Pipeline::destroy()
	{
		vkDestroyPipeline( m_device, m_pipeline, nullptr );
	}

	void Pipeline::add_shader( VkShaderStageFlagBits stage, VkShaderModule module )
	{
		VkPipelineShaderStageCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		info.pNext = nullptr;

		info.stage = stage;
		info.module = module;
		info.pName = "main";

		m_shader_stages.push_back( info );
	}

	void Pipeline::create_vertex_input( std::vector<VkVertexInputBindingDescription>& input_binding, std::vector<VkVertexInputAttributeDescription>& input_attributes )
	{
		m_vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		m_vertex_input_info.pNext = nullptr;

		m_vertex_input_info.vertexBindingDescriptionCount = input_binding.size();
		m_vertex_input_info.pVertexBindingDescriptions = input_binding.data();

		m_vertex_input_info.vertexAttributeDescriptionCount = input_attributes.size();
		m_vertex_input_info.pVertexAttributeDescriptions = input_attributes.data();
	}
	
	void Pipeline::create_assembly_info( VkPrimitiveTopology topology )
	{
		m_input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		m_input_assembly.pNext = nullptr;
		m_input_assembly.flags = 0;

		m_input_assembly.topology = topology;
		m_input_assembly.primitiveRestartEnable = VK_FALSE;
	}

	void Pipeline::create_rasterization( VkPolygonMode polygon_mode )
	{
		m_rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		m_rasterizer.pNext = nullptr;

		m_rasterizer.depthClampEnable = VK_FALSE;
		m_rasterizer.rasterizerDiscardEnable = VK_FALSE;

		m_rasterizer.polygonMode = polygon_mode;
		m_rasterizer.lineWidth = 1.0f;
		m_rasterizer.cullMode = VK_CULL_MODE_NONE;
		m_rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		
		m_rasterizer.depthBiasEnable = VK_FALSE;
		m_rasterizer.depthBiasConstantFactor = 0.0f;
		m_rasterizer.depthBiasClamp = 0.0f;
		m_rasterizer.depthBiasSlopeFactor = 0.0f;
	}
	
	void Pipeline::create_multisampling_state()
	{
		m_multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		m_multisampling.pNext = nullptr;

		m_multisampling.sampleShadingEnable = VK_FALSE;
		m_multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		m_multisampling.minSampleShading = 1.0f;
		m_multisampling.pSampleMask = nullptr;
		m_multisampling.alphaToCoverageEnable = VK_FALSE;
		m_multisampling.alphaToOneEnable = VK_FALSE;
	}

	void Pipeline::create_color_blend( std::vector<VkPipelineColorBlendAttachmentState>& m_color_blend_attachments )
	{
		m_color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		m_color_blend_state.pNext = nullptr;

		m_color_blend_state.logicOpEnable = VK_FALSE;
		m_color_blend_state.logicOp = VK_LOGIC_OP_COPY;
		m_color_blend_state.attachmentCount = m_color_blend_attachments.size();
		m_color_blend_state.pAttachments = m_color_blend_attachments.data();
	}

	void Pipeline::create_viewport_state()
	{
		m_viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		m_viewport_state.pNext = nullptr;

		m_viewport_state.viewportCount = 1;
		m_viewport_state.pViewports = &m_viewport;
		m_viewport_state.scissorCount = 1;
		m_viewport_state.pScissors = &m_scissor;
	}

	void Pipeline::set_viewport( float x, float y, float width, float height, float min_depth, float max_depth )
	{
		m_viewport.x = x;
		m_viewport.y = y;
		m_viewport.width = width;
		m_viewport.height = height;
		m_viewport.minDepth = min_depth;
		m_viewport.maxDepth = max_depth;
	}
	
	void Pipeline::set_scissor( int32_t x, int32_t y, uint32_t width, uint32_t height )
	{
		m_scissor.offset = { .x = x, .y = y };
		m_scissor.extent = { .width = width, .height = height };
	}
	
	void PipelineLayout::init( VkDevice device, std::vector<VkDescriptorSetLayout>& descriptor_set_layouts, std::vector<VkPushConstantRange>& push_constant_ranges )
	{
		m_device = device;

		VkPipelineLayoutCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		info.pNext = nullptr;

		info.flags = 0;
		info.setLayoutCount = descriptor_set_layouts.size();
		info.pSetLayouts = descriptor_set_layouts.data();
		info.pushConstantRangeCount = push_constant_ranges.size();
		info.pPushConstantRanges = push_constant_ranges.data();

		VK_CHECK(
			vkCreatePipelineLayout( m_device, &info, nullptr, &m_pipeline_layout )
		);
	}

	void PipelineLayout::destroy()
	{
		vkDestroyPipelineLayout( m_device, m_pipeline_layout, nullptr );
	}
}
