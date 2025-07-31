#pragma once

#include "Shader.h"
#include "Context.h"
#include "Device.h"
#include "RenderRegion.h"
#include "SwapChainFormat.h"
#include "DescriptorSetLayout.h"

namespace Graphics
{
    class MeshPipeline {
    public:
        class ShaderBundle {
        private:
            const Shader* m_taskShader = nullptr;
            const Shader* m_meshShader = nullptr;
            const Shader* m_fragmentShader = nullptr;

        public:
            ShaderBundle() = default;

            ShaderBundle& addShader(const Shader& shader) {
                switch (shader.getType()) {
                case Shader::Type::Task:
                    m_taskShader = &shader;
                    break;
                case Shader::Type::Mesh:
                    m_meshShader = &shader;
                    break;
                case Shader::Type::Fragment:
                    m_fragmentShader = &shader;
                    break;
                default:
                    throw std::runtime_error("Unsupported shader type for mesh pipeline.");
                }
                return *this;
            }

            bool hasTaskShader() const { return m_taskShader != nullptr; }
            bool hasMeshShader() const { return m_meshShader != nullptr; }
            bool hasFragmentShader() const { return m_fragmentShader != nullptr; }

            const Shader& getTaskShader() const { return *m_taskShader; }
            const Shader& getMeshShader() const { return *m_meshShader; }
            const Shader& getFragmentShader() const { return *m_fragmentShader; }
        };

    private:
        ShaderBundle m_shaders;
        vk::Pipeline m_pipeline = nullptr;
        vk::PipelineLayout m_pipelineLayout = nullptr;
        bool m_initialized = false;

    public:
        MeshPipeline() = default;

        MeshPipeline(const Context& context, const Device& device, const RenderRegion& canvas,
            const SwapChainFormat& format, ShaderBundle shaders,
            const std::vector<const DescriptorSetLayout*>& layouts)
            : m_shaders(std::move(shaders)) {

            if (!m_shaders.hasMeshShader()) {
                throw std::runtime_error("Mesh shader is required for mesh pipeline.");
            }

            std::vector<vk::PipelineShaderStageCreateInfo> stages;

            if (m_shaders.hasTaskShader()) {
                stages.push_back({
                    {}, vk::ShaderStageFlagBits::eTaskEXT,
                    m_shaders.getTaskShader().getModule(), "main"
                    });
            }

            stages.push_back({
                {}, vk::ShaderStageFlagBits::eMeshEXT,
                m_shaders.getMeshShader().getModule(), "main"
                });

            if (m_shaders.hasFragmentShader()) {
                stages.push_back({
                    {}, vk::ShaderStageFlagBits::eFragment,
                    m_shaders.getFragmentShader().getModule(), "main"
                    });
            }

            auto layoutsRaw = convert<vk::DescriptorSetLayout>(
                layouts, [](const DescriptorSetLayout* layout) {
                    return layout->getLayout();
                });

            vk::PipelineLayoutCreateInfo layoutInfo{};
            layoutInfo.setLayoutCount = static_cast<uint32_t>(layoutsRaw.size());
            layoutInfo.pSetLayouts = layoutsRaw.data();

            m_pipelineLayout = device.getDevice()
                .createPipelineLayout(layoutInfo, nullptr, context.getDispatchLoader());

            std::vector<vk::DynamicState> dynamicStates = {
                vk::DynamicState::eViewport,
                vk::DynamicState::eScissor
            };

            vk::PipelineDynamicStateCreateInfo dynamicState{};
            dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
            dynamicState.pDynamicStates = dynamicStates.data();

            vk::PipelineViewportStateCreateInfo viewportState{};
            viewportState.viewportCount = 1;
            viewportState.scissorCount = 1;

            vk::PipelineRasterizationStateCreateInfo rasterizer{};
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode = vk::PolygonMode::eFill;
            rasterizer.cullMode = vk::CullModeFlagBits::eBack;
            rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
            rasterizer.lineWidth = 1.0f;

            vk::PipelineMultisampleStateCreateInfo multisampling{};
            multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

            vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
            colorBlendAttachment.colorWriteMask =
                vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
            colorBlendAttachment.blendEnable = VK_FALSE;

            vk::PipelineColorBlendStateCreateInfo colorBlending{};
            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments = &colorBlendAttachment;

            vk::PipelineDepthStencilStateCreateInfo depthStencil{};
            if (format.getDepthFormat() != vk::Format::eUndefined) {
                depthStencil.depthTestEnable = VK_TRUE;
                depthStencil.depthWriteEnable = VK_TRUE;
                depthStencil.depthCompareOp = vk::CompareOp::eLess;
            }

            vk::GraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.stageCount = static_cast<uint32_t>(stages.size());
            pipelineInfo.pStages = stages.data();
            pipelineInfo.pViewportState = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState = &multisampling;
            pipelineInfo.pDepthStencilState =
                format.getDepthFormat() != vk::Format::eUndefined ? &depthStencil : nullptr;
            pipelineInfo.pColorBlendState = &colorBlending;
            pipelineInfo.pDynamicState = &dynamicState;
            pipelineInfo.layout = m_pipelineLayout;
            pipelineInfo.renderPass = VK_NULL_HANDLE;
            pipelineInfo.subpass = 0;

            pipelineInfo.flags |= vk::PipelineCreateFlagBits::eAllowDerivatives;

            auto result = device.getDevice().createGraphicsPipeline(nullptr, pipelineInfo, nullptr, context.getDispatchLoader());
            if (result.result != vk::Result::eSuccess) {
                throw std::runtime_error("Failed to create mesh pipeline!");
            }

            m_pipeline = result.value;
            m_initialized = true;
        }

        MeshPipeline(MeshPipeline&& other) noexcept {
            *this = std::move(other);
        }

        MeshPipeline& operator=(MeshPipeline&& other) noexcept {
            if (this != &other) {
                assert(!m_initialized && "MeshPipeline::operator= - already initialized");
                m_pipeline = std::exchange(other.m_pipeline, nullptr);
                m_pipelineLayout = std::exchange(other.m_pipelineLayout, nullptr);
                m_initialized = std::exchange(other.m_initialized, false);
                m_shaders = std::move(other.m_shaders);
            }
            return *this;
        }

        ~MeshPipeline() {
            assert(!m_initialized && "MeshPipeline was not destroyed!");
        }

        void destroy(const Context& context, const Device& device) {
            if (!m_initialized) return;

            device.getDevice().destroyPipelineLayout(m_pipelineLayout, nullptr, context.getDispatchLoader());
            device.getDevice().destroyPipeline(m_pipeline, nullptr, context.getDispatchLoader());
            m_initialized = false;
        }

        const vk::Pipeline& getPipeline() const { return m_pipeline; }
        const vk::PipelineLayout& getLayout() const { return m_pipelineLayout; }

        static inline const auto& getBindPoint() { return vk::PipelineBindPoint::eGraphics; };
    };
}

