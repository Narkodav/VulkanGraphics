#pragma once
#include "../Common.h"
#include "Context.h"
#include "Device.h"
#include "RenderRegion.h"
#include "SwapChain.h"
#include "Shader.h"
#include "RenderPass.h"
#include "../BufferDataLayouts.h"
#include "DescriptorSetLayout.h"

namespace Graphics
{
    class ComputePipeline
    {
    private:
        const Shader* m_computeShader = nullptr;
        vk::Pipeline m_pipeline = nullptr;
        vk::PipelineLayout m_pipelineLayout = nullptr;
        bool m_initialized = false;

    public:
        ComputePipeline() = default;

        ComputePipeline(const Context& instance, const Device& device,
            const Shader& computeShader,
            const std::vector<const DescriptorSetLayout*>& layouts,
            const std::vector<vk::PushConstantRange>& pushConstantRanges = {})
        {
            m_computeShader = &computeShader;

            auto layoutsRaw = convert<vk::DescriptorSetLayout>
                (layouts, [](const DescriptorSetLayout* layout) {
                if (layout == nullptr) return vk::DescriptorSetLayout{};
                return layout->getLayout();
                    });

            vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = vk::StructureType::ePipelineLayoutCreateInfo;
            pipelineLayoutInfo.setLayoutCount = layoutsRaw.size();
            pipelineLayoutInfo.pSetLayouts = layoutsRaw.data();
            pipelineLayoutInfo.pushConstantRangeCount = pushConstantRanges.size();
            pipelineLayoutInfo.pPushConstantRanges = pushConstantRanges.data();

            try {
                m_pipelineLayout = device.getDevice()
                    .createPipelineLayout(pipelineLayoutInfo, nullptr, instance.getDispatchLoader());
            }
            catch (const vk::SystemError& e) {
                throw std::runtime_error("failed to create compute pipeline layout: " + std::string(e.what()));
            }

            vk::PipelineShaderStageCreateInfo computeShaderStageInfo{};
            computeShaderStageInfo.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
            computeShaderStageInfo.stage = vk::ShaderStageFlagBits::eCompute;
            computeShaderStageInfo.module = computeShader.getModule();
            computeShaderStageInfo.pName = "main";

            vk::ComputePipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType = vk::StructureType::eComputePipelineCreateInfo;
            pipelineInfo.stage = computeShaderStageInfo;
            pipelineInfo.layout = m_pipelineLayout;

            auto result = device.getDevice().createComputePipeline(nullptr, pipelineInfo, nullptr, instance.getDispatchLoader());
            if (result.result != vk::Result::eSuccess) {
                throw std::runtime_error("Failed to create compute pipeline!");
            }

            m_pipeline = result.value;
            m_initialized = true;
        }

        ComputePipeline(ComputePipeline&& other) noexcept {
            m_computeShader = std::exchange(other.m_computeShader, nullptr);
            m_pipeline = std::exchange(other.m_pipeline, nullptr);
            m_pipelineLayout = std::exchange(other.m_pipelineLayout, nullptr);
            m_initialized = std::exchange(other.m_initialized, false);
        }

        ComputePipeline& operator=(ComputePipeline&& other) noexcept {
            if (this == &other) return *this;

            assert(!m_initialized && "ComputePipeline::operator=() - Pipeline already initialized");

            m_computeShader = std::exchange(other.m_computeShader, nullptr);
            m_pipeline = std::exchange(other.m_pipeline, nullptr);
            m_pipelineLayout = std::exchange(other.m_pipelineLayout, nullptr);
            m_initialized = std::exchange(other.m_initialized, false);

            return *this;
        }

        ComputePipeline(const ComputePipeline&) = delete;
        ComputePipeline& operator=(const ComputePipeline&) = delete;

        ~ComputePipeline() {
            assert(!m_initialized && "ComputePipeline was not destroyed!");
        }

        void destroy(const Context& instance, const Device& device) {
            if (!m_initialized) return;

            device.getDevice().destroyPipelineLayout(m_pipelineLayout, nullptr, instance.getDispatchLoader());
            device.getDevice().destroyPipeline(m_pipeline, nullptr, instance.getDispatchLoader());
#ifdef _DEBUG
            std::cout << "Destroyed ComputePipeline" << std::endl;
#endif
            m_initialized = false;
        }

        const vk::Pipeline& getPipeline() const { return m_pipeline; }
        const vk::PipelineLayout& getLayout() const { return m_pipelineLayout; }

        static inline const auto& getBindPoint() { return vk::PipelineBindPoint::eCompute; };
    };
}


