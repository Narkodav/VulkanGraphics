#pragma once
#include "../Rendering/Flags.h"

enum class QueueProperty : size_t {
    QUEUE_FLAGS,                                // expects std::any containing QueueFlags::Flags
    QUEUE_COUNT,                                // expects std::any containing uint32_t (min amount of queues)
    TIMESTAMP_VALID_BITS,                       // expects std::any containing uint32_t
    MIN_IMAGE_TRANSFER_GRANULARITY,             // expects std::any containing std::array<uint32_t, 3>

    PROPERTIES_NUM
};

template<QueueProperty F>
struct QueuePropertyTypeTrait;
template<> struct QueuePropertyTypeTrait<QueueProperty::QUEUE_FLAGS>
{
    using Type = QueueFlags::Flags;
};
template<> struct QueuePropertyTypeTrait<QueueProperty::QUEUE_COUNT>
{
    using Type = uint32_t;
};
template<> struct QueuePropertyTypeTrait<QueueProperty::TIMESTAMP_VALID_BITS>
{
    using Type = uint32_t;
};
template<> struct QueuePropertyTypeTrait<QueueProperty::MIN_IMAGE_TRANSFER_GRANULARITY>
{
    using Type = std::array<uint32_t, 3>;
};