#pragma once
#include <vulkan/vulkan.hpp>

#include <optional>
#include <functional>
#include <any>
#include <map>
#include <string>

#include "Window.h"

enum class QueuePropertyType : size_t {
    PRESENT_SUPPORT,                            // expects std::any containing VkBool32/bool true if needed false if unneeded
    GRAPHICS_SUPPORT,                           // expects std::any containing VkBool32/bool true if needed false if unneeded
    COMPUTE_SUPPORT,                            // expects std::any containing VkBool32/bool true if needed false if unneeded
    TRANSFER_SUPPORT,                           // expects std::any containing VkBool32/bool true if needed false if unneeded
    SPARSE_BINDING_SUPPORT,                     // expects std::any containing VkBool32/bool true if needed false if unneeded
    PROTECTED_SUPPORT,                          // expects std::any containing VkBool32/bool true if needed false if unneeded
    QUEUE_COUNT,                                // expects std::any containing uint32_t (min amount of queues)
    TIMESTAMP_VALID_BITS,                       // expects std::any containing uint32_t
    MIN_IMAGE_TRANSFER_GRANULARITY,             // expects std::any containing std::array<uint32_t, 3>

    PROPERTIES_NUM
};

class QueueCaching
{
public:

    using QueuePropertyMap = std::map<QueuePropertyType, std::any>;

    struct PropertyBundle {
        std::function<std::any(const vk::QueueFamilyProperties2&)> existsCheck;
        std::function<bool(const std::any&, const std::any&)> thresholdCompare;
        std::string name;
        int score;
    };

    static inline const std::unordered_map<QueuePropertyType, PropertyBundle> queuePropertyCheckTable{

    {QueuePropertyType::GRAPHICS_SUPPORT, {
            [](const vk::QueueFamilyProperties2& props) {
            return (props.queueFamilyProperties.queueFlags & vk::QueueFlagBits::eGraphics)
            == vk::QueueFlagBits::eGraphics;
        },
            [](const std::any& required, const std::any& available) {
            return std::any_cast<bool>(available) == std::any_cast<bool>(required);
        },
        "Graphics Queue Support",
        100
    }},

    {QueuePropertyType::COMPUTE_SUPPORT, {
            [](const vk::QueueFamilyProperties2& props) {
            return (props.queueFamilyProperties.queueFlags & vk::QueueFlagBits::eCompute)
            == vk::QueueFlagBits::eCompute;
        },
            [](const std::any& required, const std::any& available) {
            return std::any_cast<bool>(available) == std::any_cast<bool>(required);
        },
        "Compute Queue Support",
        75
    }},

    {QueuePropertyType::TRANSFER_SUPPORT, {
            [](const vk::QueueFamilyProperties2& props) {
            return (props.queueFamilyProperties.queueFlags & vk::QueueFlagBits::eTransfer)
            == vk::QueueFlagBits::eTransfer;
        },
            [](const std::any& required, const std::any& available) {
            return std::any_cast<bool>(available) == std::any_cast<bool>(required);
        },
        "Transfer Queue Support",
        75
    }},

    {QueuePropertyType::SPARSE_BINDING_SUPPORT, {
            [](const vk::QueueFamilyProperties2& props) {
            return (props.queueFamilyProperties.queueFlags & vk::QueueFlagBits::eSparseBinding)
            == vk::QueueFlagBits::eSparseBinding;
        },
            [](const std::any& required, const std::any& available) {
            return std::any_cast<bool>(available) == std::any_cast<bool>(required);
        },
        "Sparse Binding Support",
        25
    }},

    {QueuePropertyType::PROTECTED_SUPPORT, {
            [](const vk::QueueFamilyProperties2& props) {
            return (props.queueFamilyProperties.queueFlags & vk::QueueFlagBits::eProtected)
            == vk::QueueFlagBits::eProtected;
        },
            [](const std::any& required, const std::any& available) {
            return std::any_cast<bool>(available) == std::any_cast<bool>(required);
        },
        "Protected Memory Support",
        25
    }},

    {QueuePropertyType::QUEUE_COUNT, {
            [](const vk::QueueFamilyProperties2& props) {
            return props.queueFamilyProperties.queueCount;
        },
            [](const std::any& required, const std::any& available) {
            return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
        },
        "Queue Count Support",
        100
    }},

    {QueuePropertyType::TIMESTAMP_VALID_BITS, {
            [](const vk::QueueFamilyProperties2& props) {
            return props.queueFamilyProperties.timestampValidBits;
        },
            [](const std::any& required, const std::any& available) {
            return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
        },
        "Timestamp Valid Bits Support",
        50
    }},

    {QueuePropertyType::MIN_IMAGE_TRANSFER_GRANULARITY, {
            [](const vk::QueueFamilyProperties2& props) {
            return props.queueFamilyProperties.minImageTransferGranularity;
        },
            [](const std::any& required, const std::any& available) {
            return std::any_cast<uint32_t>(available) >= std::any_cast<uint32_t>(required);
        },
        "Image Transfer Granularity Support",
        50
    }}
    };



    static std::map<QueuePropertyType, std::any> getProperties(const vk::QueueFamilyProperties2& properties)
    {
        std::map<QueuePropertyType, std::any> queuePropertyMap;

        for(int i = 0; i < static_cast<int>(QueuePropertyType::PROPERTIES_NUM); i++)
            queuePropertyMap[static_cast<QueuePropertyType>(i)] =
            queuePropertyCheckTable.at(static_cast<QueuePropertyType>(i)).existsCheck(properties);

        return queuePropertyMap;
    }

    static bool compare(Rating& rating,
        const QueuePropertyMap& required,
        const QueuePropertyMap& available,
        const Window* windowFlag = nullptr)
    {


        for (auto& feature : required)
        {
            if (feature.first == QueuePropertyType::PRESENT_SUPPORT)
            {

            }

            auto it = available.find(feature.first);
            if (it != available.end() &&
                queuePropertyCheckTable.at(feature.first).thresholdCompare(feature, it->second))
            {
                rating.suitable = false;
                rating.reasons.push_back(
                    "Queue property " + queuePropertyCheckTable.at(it->first).name + " not supported");
            }
            //else rating.score += queuePropertyCheckTable.at(it->first).score;
        }

        //for (auto& feature : available)
        //    rating.score += queuePropertyCheckTable.at(feature.first).score;

        return rating.suitable;
    }

    //static bool evaluateQueues(Rating& rating,
    //    const QueuePropertyMap& required,
    //    const QueuePropertyMap& available)
    //{
    //    for (auto& feature : required)
    //    {
    //        auto it = available.find(feature.first);
    //        if (it != available.end() &&
    //            queuePropertyCheckTable.at(feature.first).thresholdCompare(feature, it->second))
    //        {
    //            rating.suitable = false;
    //            rating.reasons.push_back(
    //                "Queue property " + queuePropertyCheckTable.at(it->first).name + " not supported");
    //        }
    //        else rating.score += queuePropertyCheckTable.at(it->first).score;
    //    }

    //    for (auto& feature : available)
    //        rating.score += queuePropertyCheckTable.at(feature.first).score;

    //    return rating.suitable;
    //}
};

