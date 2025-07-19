#include "DescriptorSet.h"

namespace Graphics {

    DescriptorSet::DescriptorSet(vk::DescriptorSet set, vk::DescriptorSetLayoutCreateInfo layout) :
        m_set(set), m_initialized(true) {

        for (int i = 0; i < layout.bindingCount; i++)
            m_layout[layout.pBindings[i].binding] = layout.pBindings[i];
    };

    void DescriptorSet::write(const Context& instance, const Device& device,
        const Buffer& buffer, uint32_t binding,
        size_t offset, size_t range)
    {
        vk::DescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffer.getBuffer();
        bufferInfo.offset = offset;
        bufferInfo.range = range;

        vk::WriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = vk::StructureType::eWriteDescriptorSet;
        descriptorWrite.dstSet = m_set;
        descriptorWrite.dstBinding = binding;
        descriptorWrite.dstArrayElement = 0;

        auto it = m_layout.find(binding);
        if (it == m_layout.end())
            throw std::runtime_error("invalid binding");

        descriptorWrite.descriptorType = it->second.descriptorType;
        descriptorWrite.descriptorCount = 1;

        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr;
        descriptorWrite.pTexelBufferView = nullptr;

        device.getDevice().updateDescriptorSets(1, &descriptorWrite, 0, nullptr, instance.getDispatchLoader());
    }

    void DescriptorSet::write(const Context& instance, const Device& device,
        const std::vector<Buffer>& buffers, uint32_t binding,
        const std::vector<size_t>& offsets, const std::vector<size_t>& ranges)
    {
        std::vector<vk::DescriptorBufferInfo> bufferInfos(buffers.size());

        for (uint32_t i = 0; i < buffers.size(); i++) {
            bufferInfos[i].buffer = buffers[i].getBuffer();
            bufferInfos[i].offset = offsets[i];
            bufferInfos[i].range = ranges[i];
        }

        vk::WriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = vk::StructureType::eWriteDescriptorSet;
        descriptorWrite.dstSet = m_set;
        descriptorWrite.dstBinding = binding;
        descriptorWrite.dstArrayElement = 0;

        auto it = m_layout.find(binding);
        if (it == m_layout.end())
            throw std::runtime_error("invalid binding");

        descriptorWrite.descriptorType = it->second.descriptorType;
        descriptorWrite.descriptorCount = buffers.size();

        descriptorWrite.pBufferInfo = bufferInfos.data();
        descriptorWrite.pImageInfo = nullptr;
        descriptorWrite.pTexelBufferView = nullptr;

        device.getDevice().updateDescriptorSets(1, &descriptorWrite, 0, nullptr, instance.getDispatchLoader());
    }

    void DescriptorSet::write(const Context& instance, const Device& device,
        const Image& image, const Sampler& sampler, uint32_t binding)
    {
        vk::DescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = image.getLayout();
        imageInfo.imageView = image.getView();
        imageInfo.sampler = sampler.getSampler();

        vk::WriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = vk::StructureType::eWriteDescriptorSet;
        descriptorWrite.dstSet = m_set;
        descriptorWrite.dstBinding = binding;
        descriptorWrite.dstArrayElement = 0;

        auto it = m_layout.find(binding);
        if (it == m_layout.end())
            throw std::runtime_error("invalid binding");

        descriptorWrite.descriptorType = it->second.descriptorType;
        descriptorWrite.descriptorCount = 1;

        descriptorWrite.pImageInfo = &imageInfo;
        descriptorWrite.pBufferInfo = nullptr;
        descriptorWrite.pTexelBufferView = nullptr;

        device.getDevice().updateDescriptorSets(1, &descriptorWrite, 0, nullptr, instance.getDispatchLoader());
    }

    void DescriptorSet::write(const Context& instance, const Device& device,
        const std::vector<Image>& images, const std::vector<const Sampler*>& samplers, uint32_t binding)
    {
        std::vector<vk::DescriptorImageInfo>imageInfos(images.size());

        for (uint32_t i = 0; i < images.size(); i++) {
            imageInfos[i].sampler = samplers[i]->getSampler();
            imageInfos[i].imageView = images[i].getView();
            imageInfos[i].imageLayout = images[i].getLayout();
        }

        vk::WriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = vk::StructureType::eWriteDescriptorSet;
        descriptorWrite.dstSet = m_set;
        descriptorWrite.dstBinding = binding;
        descriptorWrite.dstArrayElement = 0;

        auto it = m_layout.find(binding);
        if (it == m_layout.end())
            throw std::runtime_error("invalid binding");

        descriptorWrite.descriptorType = it->second.descriptorType;
        descriptorWrite.descriptorCount = images.size();

        descriptorWrite.pImageInfo = imageInfos.data();
        descriptorWrite.pBufferInfo = nullptr;
        descriptorWrite.pTexelBufferView = nullptr;

        device.getDevice().updateDescriptorSets(1, &descriptorWrite, 0, nullptr, instance.getDispatchLoader());
    }
}