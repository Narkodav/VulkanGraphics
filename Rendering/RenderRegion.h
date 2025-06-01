#pragma once
#include "Common.h"
#include "PlatformManagement/Window.h"

//The rectangle where the pipeline will render to
struct RenderRegion {
    vk::Viewport viewport;
    vk::Rect2D scissor;

    RenderRegion() = default;
    ~RenderRegion() = default;

    RenderRegion(const RenderRegion& other) = default;
    RenderRegion(RenderRegion&& other) noexcept = default;

    RenderRegion& operator=(const RenderRegion& other) = default;
    RenderRegion& operator=(RenderRegion&& other) noexcept = default;

    // Create a full-window render region
    static RenderRegion createFullWindow(const Window& window) {
        RenderRegion region{};
        const auto& extent = window.getFrameBufferExtent();
        region.viewport = vk::Viewport(
            0.0f,                               // x
            0.0f,                               // y
            static_cast<float>(extent.width),   // width
            static_cast<float>(extent.height),  // height
            0.0f,                               // minDepth
            1.0f                                // maxDepth
        );

        region.scissor = vk::Rect2D(
            vk::Offset2D(0,0), extent);

        return region;
    }

    // Create a custom region within the window
    static RenderRegion createCustom(
        float x, float y,           //region left top corner
        float width, float height   //region dimensions
        )
    {
        RenderRegion region{};
        region.viewport = vk::Viewport(x, y, width, height,
            0.0f, 1.0f);
        region.scissor = vk::Rect2D(
            vk::Offset2D(static_cast<int32_t>(x), static_cast<int32_t>(y)),
            vk::Extent2D(static_cast<uint32_t>(width), static_cast<uint32_t>(height)));
        return region;
    }

    bool isWithinBounds(const Window& window) const {
        const auto& extent = window.getFrameBufferExtent();
        return (viewport.x >= 0.0f &&
            viewport.y >= 0.0f &&
            viewport.x + viewport.width <= static_cast<float>(extent.width) &&
            viewport.y + viewport.height <= static_cast<float>(extent.height));
    }
};

