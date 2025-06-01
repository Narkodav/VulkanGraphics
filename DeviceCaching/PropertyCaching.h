#pragma once
#include <vulkan/vulkan.hpp>

#include <optional>
#include <functional>
#include <any>
#include <map>
#include <string>

enum class PropertyType : size_t {
    // Core Limits
    MAX_IMAGE_DIMENSION_2D,                     // expects std::any containing uint32_t
    MAX_IMAGE_DIMENSION_3D,                     // expects std::any containing uint32_t
    MAX_IMAGE_DIMENSION_CUBE,                   // expects std::any containing uint32_t
    MAX_IMAGE_ARRAY_LAYERS,                     // expects std::any containing uint32_t

    // Descriptor Limits
    MAX_BOUND_DESCRIPTOR_SETS,                  // expects std::any containing uint32_t
    MAX_PER_STAGE_DESCRIPTOR_SAMPLERS,          // expects std::any containing uint32_t
    MAX_PER_STAGE_DESCRIPTOR_UNIFORM_BUFFERS,   // expects std::any containing uint32_t
    MAX_PER_STAGE_DESCRIPTOR_STORAGE_BUFFERS,   // expects std::any containing uint32_t

    // Buffer Limits
    MAX_STORAGE_BUFFER_RANGE,                   // expects std::any containing uint64_t (VkDeviceSize)
    MAX_UNIFORM_BUFFER_RANGE,                   // expects std::any containing uint64_t (VkDeviceSize)
    MIN_UNIFORM_BUFFER_OFFSET_ALIGNMENT,        // expects std::any containing uint64_t (VkDeviceSize)
    MIN_STORAGE_BUFFER_OFFSET_ALIGNMENT,        // expects std::any containing uint64_t (VkDeviceSize)

    // Viewport/Scissor
    MAX_VIEWPORT_DIMENSIONS,                    // expects std::any containing std::array<uint32_t, 2>
    MAX_VIEWPORTS,                              // expects std::any containing uint32_t
    VIEWPORT_BOUNDS_RANGE,                      // expects std::any containing std::array<float, 2>
    VIEWPORT_SUB_PIXEL_BITS,                    // expects std::any containing uint32_t

    // Sample Limits
    MAX_FRAMEBUFFER_LAYERS,                     // expects std::any containing uint32_t
    MAX_SAMPLE_MASK_WORDS,                      // expects std::any containing uint32_t
    MAX_COLOR_ATTACHMENTS,                      // expects std::any containing uint32_t

    // Memory Limits
    MAX_MEMORY_ALLOCATION_COUNT,                // expects std::any containing uint32_t
    MAX_SAMPLER_ALLOCATION_COUNT,               // expects std::any containing uint32_t
    BUFFER_IMAGE_GRANULARITY,                   // expects std::any containing uint64_t (VkDeviceSize)
    SPARSE_ADDRESS_SPACE_SIZE,                  // expects std::any containing uint64_t (VkDeviceSize)

    // Quality Settings
    MAX_SAMPLER_ANISOTROPY,                     // expects std::any containing float
    MAX_SAMPLER_LOD_BIAS,                       // expects std::any containing float
    TIMESTAMP_PERIOD,                           // expects std::any containing float

    PROPERTIES_NUM,
};

class PropertyCaching
{
public:

    using Properties = vk::PhysicalDeviceProperties2;
    using PropertyMap = std::map<PropertyType, std::any>;

    struct PropertyBundle {
        std::function<std::any(const Properties&)> existsCheck;
        std::function<bool(const std::any&, const std::any&)> thresholdCompare;
        std::string name;
        int score;
    };

    static inline const std::unordered_map<PropertyType, PropertyBundle> propertyBundleTable{
        // Core Limits
        {PropertyType::MAX_IMAGE_DIMENSION_2D, {
            [](const Properties& props) {
                return props.properties.limits.maxImageDimension2D;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
            },
            "2D Image Dimension Support",
            100
        }},
        {PropertyType::MAX_IMAGE_DIMENSION_3D, {
            [](const Properties& props) {
                return props.properties.limits.maxImageDimension3D;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
            },
            "3D Image Dimension Support",
            75
        }},
        {PropertyType::MAX_IMAGE_DIMENSION_CUBE, {
            [](const Properties& props) {
                return props.properties.limits.maxImageDimensionCube;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
            },
            "Cube Image Dimension Support",
            75
        }},
        {PropertyType::MAX_IMAGE_ARRAY_LAYERS, {
            [](const Properties& props) {
                return props.properties.limits.maxImageArrayLayers;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
            },
            "Image Array Layer Support",
            75
        }},

        // Descriptor Limits
        {PropertyType::MAX_BOUND_DESCRIPTOR_SETS, {
            [](const Properties& props) {
                return props.properties.limits.maxBoundDescriptorSets;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
            },
            "Bound Descriptor Sets Support",
            100
        }},
        {PropertyType::MAX_PER_STAGE_DESCRIPTOR_SAMPLERS, {
            [](const Properties& props) {
                return props.properties.limits.maxPerStageDescriptorSamplers;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
            },
            "Per-Stage Sampler Descriptor Support",
            75
        }},
        {PropertyType::MAX_PER_STAGE_DESCRIPTOR_UNIFORM_BUFFERS, {
            [](const Properties& props) {
                return props.properties.limits.maxPerStageDescriptorUniformBuffers;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
            },
            "Per-Stage Uniform Buffer Descriptor Support",
            75
        }},
        {PropertyType::MAX_PER_STAGE_DESCRIPTOR_STORAGE_BUFFERS, {
            [](const Properties& props) {
                return props.properties.limits.maxPerStageDescriptorStorageBuffers;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
            },
            "Per-Stage Storage Buffer Descriptor Support",
            75
        }},

        // Buffer Limits
        {PropertyType::MAX_STORAGE_BUFFER_RANGE, {
            [](const Properties& props) {
                return props.properties.limits.maxStorageBufferRange;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
            },
            "Storage Buffer Range Support",
            75
        }},
        {PropertyType::MAX_UNIFORM_BUFFER_RANGE, {
            [](const Properties& props) {
                return props.properties.limits.maxUniformBufferRange;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
            },
            "Uniform Buffer Range Support",
            75
        }},
        {PropertyType::MIN_UNIFORM_BUFFER_OFFSET_ALIGNMENT, {
            [](const Properties& props) {
                return props.properties.limits.minUniformBufferOffsetAlignment;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) <= std::any_cast<uint32_t>(required);
            },
            "Uniform Buffer Alignment Support",
            50
        }},
        {PropertyType::MIN_STORAGE_BUFFER_OFFSET_ALIGNMENT, {
            [](const Properties& props) {
                return props.properties.limits.minStorageBufferOffsetAlignment;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) <= std::any_cast<uint32_t>(required);
            },
            "Storage Buffer Alignment Support",
            50
        }},

        // Viewport/Scissor
        {PropertyType::MAX_VIEWPORT_DIMENSIONS, {
            [](const Properties& props) {
                return props.properties.limits.maxViewportDimensions;
            },
            [](const std::any& required, const std::any& available) {
                const auto& requiredCast = std::any_cast<std::array<uint32_t, 2>>(required);
                const auto& availableCast = std::any_cast<std::array<uint32_t, 2>>(available);
                return availableCast[0] >= requiredCast[0] &&
                    availableCast[1] >= requiredCast[1];
            },
            "Viewport Dimensions Support",
            100
        }},
        {PropertyType::MAX_VIEWPORTS, {
            [](const Properties& props) {
                return props.properties.limits.maxViewports;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
            },
            "Multiple Viewport Support",
            75
        }},
        {PropertyType::VIEWPORT_BOUNDS_RANGE, {
            [](const Properties& props) {
                return props.properties.limits.viewportBoundsRange;
            },
            [](const std::any& required, const std::any& available) {
                const auto& requiredCast = std::any_cast<std::array<uint32_t, 2>>(required);
                const auto& availableCast = std::any_cast<std::array<uint32_t, 2>>(available);
                return availableCast[0] <= requiredCast[0] &&
                    availableCast[1] >= requiredCast[1];
            },
            "Viewport Bounds Range Support",
            50
        }},
        {PropertyType::VIEWPORT_SUB_PIXEL_BITS, {
            [](const Properties& props) {
                return props.properties.limits.viewportSubPixelBits;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
            },
            "Viewport Sub-Pixel Precision Support",
            25
        }},

        // Sample Limits
        {PropertyType::MAX_FRAMEBUFFER_LAYERS, {
            [](const Properties& props) {
                return props.properties.limits.maxFramebufferLayers;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
            },
            "Framebuffer Layer Support",
            75
        }},
        {PropertyType::MAX_SAMPLE_MASK_WORDS, {
            [](const Properties& props) {
                return props.properties.limits.maxSampleMaskWords;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
            },
            "Sample Mask Support",
            50
        }},
        {PropertyType::MAX_COLOR_ATTACHMENTS, {
            [](const Properties& props) {
                return props.properties.limits.maxColorAttachments;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
            },
            "Color Attachment Support",
            100
        }},

        // Memory Limits
        {PropertyType::MAX_MEMORY_ALLOCATION_COUNT, {
            [](const Properties& props) {
                return props.properties.limits.maxMemoryAllocationCount;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
            },
            "Memory Allocation Count Support",
            75
        }},
        {PropertyType::MAX_SAMPLER_ALLOCATION_COUNT, {
            [](const Properties& props) {
                return props.properties.limits.maxSamplerAllocationCount;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
            },
            "Sampler Allocation Support",
            50
        }},
        {PropertyType::BUFFER_IMAGE_GRANULARITY, {
            [](const Properties& props) {
                return props.properties.limits.bufferImageGranularity;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) <= std::any_cast<uint32_t>(required);
            },
            "Buffer Image Granularity Support",
            50
        }},
        {PropertyType::SPARSE_ADDRESS_SPACE_SIZE, {
            [](const Properties& props) {
                return props.properties.limits.sparseAddressSpaceSize;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
            },
            "Sparse Address Space Support",
            25
        }},

        // Quality Settings
        {PropertyType::MAX_SAMPLER_ANISOTROPY, {
            [](const Properties& props) {
                return props.properties.limits.maxSamplerAnisotropy;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
            },
            "Anisotropic Filtering Support",
            75
        }},
        {PropertyType::MAX_SAMPLER_LOD_BIAS, {
            [](const Properties& props) {
                return props.properties.limits.maxSamplerLodBias;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
            },
            "Sampler LOD Bias Support",
            50
        }},
        {PropertyType::TIMESTAMP_PERIOD, {
            [](const Properties& props) {
                return props.properties.limits.timestampPeriod;
            },
            [](const std::any& required, const std::any& available) {
                return std::any_cast<uint32_t>(available) <= std::any_cast<uint32_t>(required);
            },
            "Timestamp Period Support",
            25
        }}
    };

    static PropertyMap getProperties(const Properties& properties)
    {

        PropertyMap propertyMap;

        for (int i = 0; i < static_cast<int>(PropertyType::PROPERTIES_NUM); i++)
            propertyMap[static_cast<PropertyType>(i)] =
            propertyBundleTable.at(static_cast<PropertyType>(i)).existsCheck(properties);

        return propertyMap;
    }

    static bool compare(Rating& rating,
        const PropertyMap& required,
        const PropertyMap& available)
    {
        for (auto& feature : required)
        {
            auto it = available.find(feature.first);
            if (it != available.end() && 
            propertyBundleTable.at(feature.first).thresholdCompare(feature, it->second))
            {
                rating.suitable = false;
                rating.reasons.push_back(
                    "Property " + propertyBundleTable.at(it->first).name + " not supported");
            }
        }

        if (!rating.suitable)
            return false;

        for (auto& feature : available)
            rating.score += propertyBundleTable.at(feature.first).score;

        return rating.suitable;
    }
};

