#include "DescriptorSet.h"

DescriptorSet::DescriptorSet(vk::DescriptorSet set, vk::DescriptorSetLayoutCreateInfo layout) :
    m_set(set), m_initialized(true) {

    for (int i = 0; i < layout.bindingCount; i++)
        m_layout[layout.pBindings[i].binding] = layout.pBindings[i];
};

void DescriptorSet::write(const EngineContext& instance, const EngineDevice& device,
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

void DescriptorSet::write(const EngineContext& instance, const EngineDevice& device,
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