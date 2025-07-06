#pragma once
#include "../Common.h"
#include "GraphicsContext.h"
#include "Device.h"
#include "RenderRegion.h"
#include "SwapChain.h"
#include "Shader.h"
#include "RenderPass.h"
#include "../BufferDataLayouts.h"
#include "DescriptorSetLayout.h"

class GraphicsPipeline
{
public:
    class ShaderBundle
    {
    private:
        std::array<const Shader*, 5> m_shaderHandles = {
            nullptr, nullptr, nullptr, nullptr, nullptr };

    public:
        ShaderBundle() = default;
        ShaderBundle(const ShaderBundle&) = default;
        ShaderBundle& operator=(const ShaderBundle&) = default;
        ShaderBundle(ShaderBundle&&) = default;
        ShaderBundle& operator=(ShaderBundle&&) = default;

        ShaderBundle& addShader(const Shader& shader)
        {
            m_shaderHandles[static_cast<size_t>(shader.getType())] = &shader;
            return *this;
        }

        template<typename... Shaders>
        ShaderBundle(const Shaders&... shaders)
        {
            ((m_shaderHandles[static_cast<size_t>(shaders.getType())] = &shaders), ...);
        }

        template <Shader::Type ShaderType>
        bool hasShader() const {
            return m_shaderHandles[static_cast<size_t>(ShaderType)] != nullptr;
        };

        template <Shader::Type ShaderType>
        const Shader& getShader() const {
            return *m_shaderHandles[static_cast<size_t>(ShaderType)];
        };
    };

private:

    ShaderBundle m_shaders;
    
    vk::Pipeline m_pipeline = nullptr;
    vk::PipelineLayout m_pipelineLayout = nullptr;
    bool m_initialized = false;

    template<Shader::Type Type>
    vk::PipelineShaderStageCreateInfo createShaderStage(vk::ShaderStageFlagBits stage) {
        vk::PipelineShaderStageCreateInfo stageInfo{};
        stageInfo.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
        stageInfo.stage = stage;
        stageInfo.module = m_shaders.getShader<Type>().getModule();
        stageInfo.pName = "main";
        return stageInfo;
    }

    std::pair<std::vector<vk::PipelineShaderStageCreateInfo>, 
    std::optional<vk::PipelineTessellationStateCreateInfo>>
        createShaderStages() {
        std::pair<std::vector<vk::PipelineShaderStageCreateInfo>,
            std::optional<vk::PipelineTessellationStateCreateInfo>> result;

        // Mandatory stages
            result.first.push_back(createShaderStage<Shader::Type::VERTEX>(
            vk::ShaderStageFlagBits::eVertex));
            result.first.push_back(createShaderStage<Shader::Type::FRAGMENT>(
            vk::ShaderStageFlagBits::eFragment));

        // Optional geometry shader
        if (m_shaders.hasShader<Shader::Type::GEOMETRY>()) {
            result.first.push_back(createShaderStage<Shader::Type::GEOMETRY>(
                vk::ShaderStageFlagBits::eGeometry));
        }

        // Optional tessellation shaders
        if (m_shaders.hasShader<Shader::Type::TESS_CONTROL>()) {
            if (!m_shaders.hasShader<Shader::Type::TESS_EVALUATION>()) {
                throw std::runtime_error("Both tessellation shaders must be provided");
            }
            result.first.push_back(createShaderStage<Shader::Type::TESS_CONTROL>(
                vk::ShaderStageFlagBits::eTessellationControl));
            result.first.push_back(createShaderStage<Shader::Type::TESS_EVALUATION>(
                vk::ShaderStageFlagBits::eTessellationEvaluation));

            vk::PipelineTessellationStateCreateInfo tessellationState{};
            tessellationState.sType = vk::StructureType::ePipelineTessellationStateCreateInfo;
            tessellationState.patchControlPoints = 3; // Or configurable
            result.second = tessellationState;
        }

        return result;
    }
public:
    GraphicsPipeline() {};

    template <VertexDefinition... VertexDefs>
    GraphicsPipeline(const GraphicsContext& instance, const Device& device, const RenderPass& renderPass,
        ShaderBundle shaders, const RenderRegion& canvas, const SwapChainFormat& format,
        const VertexDefinitions<VertexDefs...>& vertices,
        const std::vector<const DescriptorSetLayout*>& layouts)
    {
        m_shaders = shaders;
        auto shaderState = createShaderStages();

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;

        auto bindingDescriptions = vertices.enumerateBindings();
        auto attributeDescriptions = vertices.enumerateAttributes();

        vertexInputInfo.vertexBindingDescriptionCount = bindingDescriptions.size();
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
        vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo;
        inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        std::vector<vk::DynamicState> dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor
        };

        vk::PipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        vk::PipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = vk::StructureType::ePipelineViewportStateCreateInfo;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;


        vk::PipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = vk::PolygonMode::eFill;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = vk::CullModeFlagBits::eBack;
        rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

        vk::PipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional

        vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask =
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne; // Optional
        colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero; // Optional
        colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd; // Optional
        colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne; // Optional
        colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero; // Optional
        colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd; // Optional

        vk::PipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = vk::LogicOp::eCopy; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional

        auto layoutsRaw = convert<vk::DescriptorSetLayout>
            (layouts, [this](const DescriptorSetLayout* layout)
                {
                    return layout->getLayout();
                });

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = vk::StructureType::ePipelineLayoutCreateInfo;
        pipelineLayoutInfo.setLayoutCount = layoutsRaw.size();  // number of descriptor set layouts
        pipelineLayoutInfo.pSetLayouts = layoutsRaw.data();  // array of descriptor set layouts
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        try {
            m_pipelineLayout = device.getDevice()
                .createPipelineLayout(pipelineLayoutInfo, nullptr, instance.getDispatchLoader());
        }
        catch (const vk::SystemError& e) {
            throw std::runtime_error("failed to create pipeline layout: " + std::string(e.what()));
        }
        catch (const std::exception& e) {
            throw std::runtime_error("Unexpected error when creating a pipeline layout: " + std::string(e.what()));
        }

        vk::GraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
        pipelineInfo.stageCount = static_cast<uint32_t>(shaderState.first.size());
        pipelineInfo.pStages = shaderState.first.data();

        if(shaderState.second.has_value())
            pipelineInfo.pTessellationState = &shaderState.second.value();

        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;

        vk::PipelineDepthStencilStateCreateInfo depthStencil{};
        if(format.getDepthFormat() != vk::Format::eUndefined)
        {
            depthStencil.depthTestEnable = VK_TRUE;
            depthStencil.depthWriteEnable = VK_TRUE;
            depthStencil.depthCompareOp = vk::CompareOp::eLess;

            pipelineInfo.pDepthStencilState = &depthStencil; // Optional
        }
        else pipelineInfo.pDepthStencilState = nullptr;

        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = m_pipelineLayout;

        pipelineInfo.renderPass = renderPass.getRenderPass();
        pipelineInfo.subpass = 0;

        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional

        auto result = device.getDevice().createGraphicsPipeline(nullptr, pipelineInfo, nullptr, instance.getDispatchLoader());
        if (result.result != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }

        m_pipeline = result.value;
        m_initialized = true;
    }

    GraphicsPipeline(GraphicsPipeline&& other) noexcept {
        m_shaders = std::exchange(other.m_shaders, ShaderBundle());
        m_pipeline = std::exchange(other.m_pipeline, nullptr);
        m_pipelineLayout = std::exchange(other.m_pipelineLayout, nullptr);
        m_initialized = std::exchange(other.m_initialized, false);
    };

    //moving to an initialized device is undefined behavior, destroy before moving
    GraphicsPipeline& operator=(GraphicsPipeline&& other) noexcept
    {
        if (this == &other)
            return *this;

        assert(!m_initialized && "GraphicsPipeline::operator=() - GraphicsPipeline already initialized");

        m_shaders = std::exchange(other.m_shaders, ShaderBundle());
        m_pipeline = std::exchange(other.m_pipeline, nullptr);
        m_pipelineLayout = std::exchange(other.m_pipelineLayout, nullptr);
        m_initialized = std::exchange(other.m_initialized, false);

        return *this;
    };

    GraphicsPipeline(const GraphicsPipeline& other) noexcept = delete;
    GraphicsPipeline& operator=(const GraphicsPipeline& other) noexcept = delete;

    ~GraphicsPipeline() { assert(!m_initialized && "GraphicsPipeline was not destroyed!"); };

    void destroy(const GraphicsContext& instance, const Device& device) {
        if (!m_initialized)
            return;

        device.getDevice().destroyPipelineLayout(m_pipelineLayout, nullptr, instance.getDispatchLoader());
        device.getDevice().destroyPipeline(m_pipeline, nullptr, instance.getDispatchLoader());
#ifdef _DEBUG
        std::cout << "Destroyed GraphicsPipeline" << std::endl;
#endif
        m_initialized = false;
    }

    const vk::Pipeline& getPipeline() const { return m_pipeline; };
    const vk::PipelineLayout& getLayout() const { return m_pipelineLayout; };
};

