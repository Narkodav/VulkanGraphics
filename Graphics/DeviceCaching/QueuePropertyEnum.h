#pragma once
#include "../Rendering/Flags.h"

namespace Graphics {

    enum class QueueProperty : size_t {
        QueueFlags,                                // expects std::any containing QueueFlags::Flags
        QueueCount,                                // expects std::any containing uint32_t (min amount of queues)
        TimestampValidBits,                       // expects std::any containing uint32_t
        MinImageTransferGranularity,             // expects std::any containing std::array<uint32_t, 3>

        PropertiesNum
    };

    template<QueueProperty F>
    struct QueuePropertyTypeTrait;
    template<> struct QueuePropertyTypeTrait<QueueProperty::QueueFlags>
    {
        using Type = QueueFlags::Flags;
    };
    template<> struct QueuePropertyTypeTrait<QueueProperty::QueueCount>
    {
        using Type = uint32_t;
    };
    template<> struct QueuePropertyTypeTrait<QueueProperty::TimestampValidBits>
    {
        using Type = uint32_t;
    };
    template<> struct QueuePropertyTypeTrait<QueueProperty::MinImageTransferGranularity>
    {
        using Type = std::array<uint32_t, 3>;
    };

}