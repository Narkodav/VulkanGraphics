#pragma once
#include "Common.h"
#include "Rendering/Flags.h"

template <typename... Types>
class TypeErasedArray
{
protected:
    std::tuple<Types...> m_data;

public:
    template<typename Func>
    void iterate(Func&& callback) {
        (... && callback(std::get<Types>(m_data)));
    }

    template<typename Func>
    void iterate(Func&& callback) const {
        (... && callback(std::get<Types>(m_data)));
    }

    template<typename Func>
    void iterateComplete(Func&& callback) {
        (callback(std::get<Types>(m_data)), ...);
    }

    template<typename Func>
    void iterateComplete(Func&& callback) const {
        (callback(std::get<Types>(m_data)), ...);
    }

    template<size_t I>
    auto& get() { return std::get<I>(m_data); }

    template<size_t I>
    const auto& get() const { return std::get<I>(m_data); }

    static size_t size() { return sizeof...(Types); };
    static bool empty() { return sizeof...(Types) == 0; };
};

template<typename T>
concept VertexDefinition = requires {
    { T::getBindingDescriptions() } -> std::same_as<ArrayInterface<vk::VertexInputBindingDescription>>;
    { T::getAttributeDescriptions() } -> std::same_as<ArrayInterface<vk::VertexInputAttributeDescription>>;
};

template<typename T>
concept DescriptorDefinition = requires {
    { T::getDescriptorSetLayoutBinding() } -> std::same_as<vk::DescriptorSetLayoutBinding>;
};

template <VertexDefinition... Defs>
class VertexDefinitions : public TypeErasedArray<Defs...> {
public:
    auto enumerateAttributes() const
    {
        std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
        this->iterateComplete([&]<VertexDefinition T>(const T & elem) {
            const auto& attributes = elem.getAttributeDescriptions();
            for (int i = 0; i < attributes.size(); i++)
                attributeDescriptions.push_back(attributes[i]);
        });
        return attributeDescriptions;
    }

    auto enumerateBindings() const
    {
        std::vector<vk::VertexInputBindingDescription> bindingDescriptions;
        this->iterateComplete([&]<VertexDefinition T>(const T & elem) {
            const auto& bindings = elem.getBindingDescriptions();
            for (int i = 0; i < bindings.size(); i++)
                bindingDescriptions.push_back(bindings[i]);
        });
        return bindingDescriptions;
    }
};

template <DescriptorDefinition... Defs>
class DescriptorDefinitions : public TypeErasedArray<Defs...> {
public:
    auto enumerateDescriptors() const
    {
        std::vector<vk::DescriptorSetLayoutBinding> descriptors;
        descriptors.reserve(this->size());
        this->iterateComplete([&]<DescriptorDefinition T>(const T & elem) {
            descriptors.push_back(elem.getDescriptorSetLayoutBinding());
        });
        return descriptors;
    }
};

struct VertexBasic
{
    glm::vec4 position;
    glm::vec4 UV;
};

struct VertexDefinitionBasic {
public:
    static constexpr size_t BINDING_COUNT = 1;
    static constexpr size_t ATTRIBUTE_COUNT = 2;
    static constexpr size_t DATA_SIZE = sizeof(VertexBasic);

    using Type = VertexBasic;

    static constexpr vk::VertexInputBindingDescription bindings[] = {
        {
            0,                                      // binding
            DATA_SIZE,                              // stride
            vk::VertexInputRate::eVertex            // input rate
        }
    };

    static constexpr vk::VertexInputAttributeDescription attributes[] = {
        {
            0,                                      // location
            0,                                      // binding
            vk::Format::eR32G32B32A32Sfloat,        // format
            0                                       // offset
        },
        {
            1,                                      // location
            0,                                      // binding
            vk::Format::eR32G32B32A32Sfloat,        // format
            sizeof(glm::vec4)                       // offset
        }
    };

    static ArrayInterface<vk::VertexInputBindingDescription> getBindingDescriptions() {
        return ArrayInterface<vk::VertexInputBindingDescription>(bindings, BINDING_COUNT);
    }

    static ArrayInterface<vk::VertexInputAttributeDescription> getAttributeDescriptions() {
        return ArrayInterface<vk::VertexInputAttributeDescription>(attributes, ATTRIBUTE_COUNT);
    }
};

struct VertexDefinitionPosition {
public:
    static constexpr size_t BINDING_COUNT = 1;
    static constexpr size_t ATTRIBUTE_COUNT = 1;
    static constexpr size_t DATA_SIZE = sizeof(glm::vec4);

    using Type = glm::vec4;

    static constexpr vk::VertexInputBindingDescription bindings[] = {
        {
            0,                                      // binding
            DATA_SIZE,                              // stride
            vk::VertexInputRate::eVertex            // input rate
        }
    };

    static constexpr vk::VertexInputAttributeDescription attributes[] = {
        {
            0,                                      // location
            0,                                      // binding
            vk::Format::eR32G32B32A32Sfloat,        // format
            0                                       // offset
        }
    };

    static ArrayInterface<vk::VertexInputBindingDescription> getBindingDescriptions() {
        return ArrayInterface<vk::VertexInputBindingDescription>(bindings, BINDING_COUNT);
    }

    static ArrayInterface<vk::VertexInputAttributeDescription> getAttributeDescriptions() {
        return ArrayInterface<vk::VertexInputAttributeDescription>(attributes, ATTRIBUTE_COUNT);
    }
};

struct VertexDefinitionUV {
public:
    static constexpr size_t BINDING_COUNT = 1;
    static constexpr size_t ATTRIBUTE_COUNT = 1;
    static constexpr size_t DATA_SIZE = sizeof(glm::vec2);

    using Type = glm::vec2;

    static constexpr vk::VertexInputBindingDescription bindings[] = {
        {
            1,                                      // binding
            DATA_SIZE,                              // stride
            vk::VertexInputRate::eVertex            // input rate
        }
    };

    static constexpr vk::VertexInputAttributeDescription attributes[] = {
        {
            1,                                      // location
            1,                                      // binding
            vk::Format::eR32G32Sfloat,              // format
            0                                       // offset
        }
    };

    static ArrayInterface<vk::VertexInputBindingDescription> getBindingDescriptions() {
        return ArrayInterface<vk::VertexInputBindingDescription>(bindings, BINDING_COUNT);
    }

    static ArrayInterface<vk::VertexInputAttributeDescription> getAttributeDescriptions() {
        return ArrayInterface<vk::VertexInputAttributeDescription>(attributes, ATTRIBUTE_COUNT);
    }
};

struct VertexDefinitionColor {
public:
    static constexpr size_t BINDING_COUNT = 1;
    static constexpr size_t ATTRIBUTE_COUNT = 1;
    static constexpr size_t DATA_SIZE = sizeof(glm::vec4);

    using Type = glm::vec4;

    static constexpr vk::VertexInputBindingDescription bindings[] = {
        {
            1,                                      // binding
            DATA_SIZE,                              // stride
            vk::VertexInputRate::eVertex            // input rate
        }
    };

    static constexpr vk::VertexInputAttributeDescription attributes[] = {
        {
            1,                                      // location
            1,                                      // binding
            vk::Format::eR32G32B32A32Sfloat,        // format
            0                                       // offset
        }
    };

    static ArrayInterface<vk::VertexInputBindingDescription> getBindingDescriptions() {
        return ArrayInterface<vk::VertexInputBindingDescription>(bindings, BINDING_COUNT);
    }

    static ArrayInterface<vk::VertexInputAttributeDescription> getAttributeDescriptions() {
        return ArrayInterface<vk::VertexInputAttributeDescription>(attributes, ATTRIBUTE_COUNT);
    }
};

struct ModelTransform
{
    glm::mat4 transform;
};

struct VertexDefinitionModelTransform {
public:
    static constexpr size_t BINDING_COUNT = 1;
    static constexpr size_t ATTRIBUTE_COUNT = 4;
    static constexpr size_t DATA_SIZE = sizeof(glm::mat4);

    using Type = glm::mat4;

    static constexpr vk::VertexInputBindingDescription bindings[] = {
        {
            2,                                      // binding
            DATA_SIZE,                              // stride
            vk::VertexInputRate::eInstance          // input rate
        }
    };

    static constexpr vk::VertexInputAttributeDescription attributes[] = {
        {
            2,                                      // location
            2,                                      // binding
            vk::Format::eR32G32B32A32Sfloat,        // format
            0                                       // offset
        },
        {
            3,                                      // location
            2,                                      // binding
            vk::Format::eR32G32B32A32Sfloat,        // format
            sizeof(glm::vec4)                       // offset
        },
        {
            4,                                      // location
            2,                                      // binding
            vk::Format::eR32G32B32A32Sfloat,        // format
            sizeof(glm::vec4) * 2                   // offset
        },
        {
            5,                                      // location
            2,                                      // binding
            vk::Format::eR32G32B32A32Sfloat,        // format
            sizeof(glm::vec4) * 3                   // offset
        }
    };

    static ArrayInterface<vk::VertexInputBindingDescription> getBindingDescriptions() {
        return ArrayInterface<vk::VertexInputBindingDescription>(bindings, BINDING_COUNT);
    }

    static ArrayInterface<vk::VertexInputAttributeDescription> getAttributeDescriptions() {
        return ArrayInterface<vk::VertexInputAttributeDescription>(attributes, ATTRIBUTE_COUNT);
    }
};

struct UniformTransforms {
    glm::mat4 view;
    glm::mat4 proj;
};

struct UniformTransformsDefinition {
    static constexpr size_t BINDING_COUNT = 1;

    static constexpr vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding = {
            0,                                      // binding
            vk::DescriptorType::eUniformBuffer,     // descriptor type
            1,                                      // descriptor count
            vk::ShaderStageFlagBits::eVertex,       // stage flags
            nullptr                                 // immutable samplers
    };

    static vk::DescriptorSetLayoutBinding
        getDescriptorSetLayoutBinding()
    {
        return descriptorSetLayoutBinding;
    };
};

struct ImageSamplerDefinition {
    static constexpr size_t BINDING_COUNT = 1;

    static constexpr vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding = {
            1,                                              // binding
            vk::DescriptorType::eCombinedImageSampler,      // descriptor type
            1,                                              // descriptor count
            vk::ShaderStageFlagBits::eFragment,             // stage flags
            nullptr                                         // immutable samplers
    };

    static vk::DescriptorSetLayoutBinding
        getDescriptorSetLayoutBinding()
    {
        return descriptorSetLayoutBinding;
    };
};