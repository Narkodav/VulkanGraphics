#pragma once
#pragma warning(error: 4715)

//glfw
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//vulkan
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>
//#include <vma/vk_mem_alloc.h>

//glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RIGHT_HANDED
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <stb_image.h>

#include <optional>
#include <functional>
#include <any>
#include <ranges>
#include <stdexcept>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <atomic>
#include <set>
#include <fstream>
#include <array>
#include <unordered_set>
#include <stack>
#include <chrono>

#include "FrameRateCalculator.h"

struct Version
{
    uint32_t major;
    uint32_t minor;
    uint32_t patch;

    Version(uint32_t maj = 1, uint32_t min = 0, uint32_t pat = 0)
        : major(maj), minor(min), patch(pat) {}

    uint32_t getVersion() const
    {
        return VK_MAKE_VERSION(major, minor, patch);
    }
};

struct Extent {
    uint32_t width;
    uint32_t height;

    Extent(uint32_t w = 800, uint32_t h = 600)
        : width(w), height(h) {}

    // Implicit cast to vk::Extent2D
    operator vk::Extent2D() const { return vk::Extent2D{ width, height }; }

    // Helper to get current window size from GLFW window
    static Extent getWindowExtent(GLFWwindow* window) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        return Extent{
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };
    }

    // Helper to get current frame buffer size from GLFW window
    static Extent getFrameBufferExtent(GLFWwindow* window) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        return Extent{
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };
    }
};

struct Rating {
    int score = 0;
    bool suitable = false;
    std::vector<std::string> reasons;  // Why device was rejected if not suitable
};

struct CopyRegion
{
    size_t srcOffset = 0;
    size_t dstOffset = 0;
    size_t size = 0;

    operator vk::BufferCopy() const {
        return vk::BufferCopy{ srcOffset, dstOffset, size };
    }

    CopyRegion() {};

    CopyRegion(const vk::BufferCopy& other) :
        srcOffset(other.srcOffset), dstOffset(other.dstOffset), size(other.size) {
    };

    CopyRegion(size_t srcOffset, size_t dstOffset, size_t size) :
        srcOffset(srcOffset), dstOffset(dstOffset), size(size) {
    };

};

struct Color
{
    float r, g, b, a;

    Color(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) :
        r(r), g(g), b(b), a(a) {};

    Color(uint8_t r, uint8_t g, uint8_t b,
        uint8_t a = std::numeric_limits<uint8_t>::max()) :
        r(static_cast<float>(r) / 255.f), 
        g(static_cast<float>(g) / 255.f), 
        b(static_cast<float>(b) / 255.f), 
        a(static_cast<float>(a) / 255.f) {
    };

    Color(const Color&) = default;
    Color(Color&&) = default;
    Color& operator=(const Color&) = default;
    Color& operator=(Color&&) = default;

    operator glm::vec4() const { return glm::vec4(r, g, b, a); };
    operator vk::ClearValue() const { return vk::ClearValue({ r, g, b, a }); };
    std::array<float, 4> toArray() const { return { r, g, b, a }; }

    static Color Black() { return Color(0.0f, 0.0f, 0.0f); }
    static Color White() { return Color(1.0f, 1.0f, 1.0f); }
    static Color Red() { return Color(1.0f, 0.0f, 0.0f); }
    static Color Green() { return Color(0.0f, 1.0f, 0.0f); }
    static Color Blue() { return Color(0.0f, 0.0f, 1.0f); }
};

//only works as an interface to existing memory, doesn't actually own it
template <typename T>
class ArrayInterface
{
private:
    const T* const m_array = nullptr;
    const size_t m_size = 0;

public:
    ArrayInterface() : m_array(nullptr), m_size(0) {};

    ArrayInterface(const T* const array, const size_t size) :
        m_array(array), m_size(size) {};

    const T& operator[](const size_t index) const {
        return m_array[index];
    }

    const T* const data() const { return m_array; };
    size_t size() const { return m_size; };
};

template<typename StateNew, typename StatePrior, typename Converter, size_t N>
static std::array<StateNew, N> convert(
    std::array<StatePrior, N> const& prior,
    Converter&& converter)
{
    std::array<StateNew, N> current;
    for (int i = 0; i < N; i++) {
        current[i] = converter(prior[i]);
    }
    return current;
}

template<typename StateNew, typename StatePrior, typename Converter>
static std::vector<StateNew> convert(
    std::vector<StatePrior> const& prior,
    Converter&& converter)
{
    std::vector<StateNew> current(prior.size());
    for (int i = 0; i < prior.size(); i++) {
        current[i] = converter(prior[i]);
    }
    return current;
}

enum class DescriptorType
{
    UNIFORM_BUFFER = vk::DescriptorType::eUniformBuffer,
    SAMPLER = vk::DescriptorType::eSampler,
    COMBINED_IMAGE_SAMPLER = vk::DescriptorType::eCombinedImageSampler,
    STORAGE_BUFFER = vk::DescriptorType::eStorageBuffer,
    STORAGE_IMAGE = vk::DescriptorType::eStorageImage,
    INPUT_ATTACHMENT = vk::DescriptorType::eInputAttachment
};