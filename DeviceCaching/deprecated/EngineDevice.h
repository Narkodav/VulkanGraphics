#pragma once
#include "Common.h"
#include "PlatformManagement/Window.h"
#include "EngineContext.h"
#include "DeviceCaching/PhysicalDeviceCache.h"

enum class QueueSpecialisation
{
    GRAPHICS,
    PRESENT,
    TRANSFER,
    COMPUTE,
    NUM
};

struct QueueCreateData
{
    vk::Queue queue;
    uint32_t index;
    QueueSpecialisation specialisation;
};

struct BasicRenderingQueueFamilyIndices
{
    std::array<std::optional<uint32_t>, static_cast<size_t>(QueueSpecialisation::NUM)> families;

    //static inline const std::vector<QueueCaching::QueuePropertyMap> graphicsQueueRequirement
    //    = { {QueuePropertyType::GRAPHICS_SUPPORT, 1, 0}, {QueuePropertyType::PRESENT_SUPPORT, 1, 0} };

    //static inline const std::vector<QueueRequirements::QueueFamilyRequirement> transferQueueRequirement
    //    = { {QueuePropertyType::TRANSFER_SUPPORT, 1, 1}, {QueuePropertyType::GRAPHICS_SUPPORT, 0, 1},
    //    {QueuePropertyType::PRESENT_SUPPORT, 0, 1} };

    //static inline const std::vector<QueueRequirements::QueueFamilyRequirement> computeQueueRequirement
    //    = { {QueuePropertyType::COMPUTE_SUPPORT, 1, 2}, {QueuePropertyType::GRAPHICS_SUPPORT, 0, 2},
    //    {QueuePropertyType::PRESENT_SUPPORT, 0, 2} };

    //static inline const std::vector<QueueRequirements::QueueFamilyRequirement>
    //    requirementBundle = []() {
    //    std::vector<QueueRequirements::QueueFamilyRequirement> vector;
    //    vector.reserve(graphicsQueueRequirement.size() +
    //        transferQueueRequirement.size() +
    //        computeQueueRequirement.size());
    //    vector.insert(vector.end(), graphicsQueueRequirement.begin(), graphicsQueueRequirement.end());
    //    vector.insert(vector.end(), transferQueueRequirement.begin(), transferQueueRequirement.end());
    //    vector.insert(vector.end(), computeQueueRequirement.begin(), computeQueueRequirement.end());
    //    return vector;
    //    }();

    //void fillQueueIndices(std::map<size_t, QueueFamilyMapping> queueMap)
    //{
    //    size_t maxScores[3] = { 0, 0, 0 };
    //    size_t indices[3] = { 0, 0, 0 };

    //    for (auto it : queueMap)
    //    {
    //        for (int i = 0; i < it.second.queueScores.size(); ++i)
    //        {
    //            if (it.second.queueScores[i] > maxScores[it.second.groupId])
    //            {
    //                maxScores[it.second.groupId] = it.second.queueScores[i];
    //                indices[it.second.groupId] = it.second.familyIndices[i];
    //            }
    //        }
    //    }

    //    graphicsFamily.emplace(indices[0]);
    //    transferFamily.emplace(indices[1]);
    //    computeFamily.emplace(indices[2]);
    //}

    bool isComplete() {
        return families[static_cast<size_t>(QueueSpecialisation::GRAPHICS)].has_value() && 
            families[static_cast<size_t>(QueueSpecialisation::PRESENT)].has_value();
    }
};

struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

//chooses a device with rendering and present support, will implement custom requirements later
class Device
{
public:
    static inline const std::vector<const char*> requiredExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

private:
    BasicRenderingQueueFamilyIndices m_basicIndices;
	vk::PhysicalDevice m_physicalDevice = nullptr;
	vk::Device m_device = nullptr;
    std::array<vk::Queue, static_cast<size_t>(QueueSpecialisation::NUM)> m_queues = { nullptr };
    SwapChainSupportDetails m_swapChainSupportDetails;

    bool m_initialized = false;

public:

    Device() : m_physicalDevice(nullptr), m_device(nullptr) {};

    Device(const EngineContext& instance, const Window& window) {
        selectPhysicalDevice(instance, window);
        createLogicalDevice(instance, window);
        m_initialized = true;
    };

    Device(Device&& other) noexcept {
        
        m_basicIndices              = std::exchange(other.m_basicIndices, {});
        m_physicalDevice            = std::exchange(other.m_physicalDevice, nullptr);
        m_device                    = std::exchange(other.m_device, nullptr);
        m_queues                    = std::exchange(other.m_queues, { nullptr });
        m_swapChainSupportDetails   = std::exchange(other.m_swapChainSupportDetails, {});
        m_initialized               = std::exchange(other.m_initialized, false);

    };

    //moving to an initialized device is undefined behavior, destroy before moving
    Device& operator=(Device&& other) noexcept
    {
        if (this == &other)
            return *this;

        assert(!m_initialized && "Device::operator=() - Device already initialized");

        m_basicIndices              = std::exchange(other.m_basicIndices, {});
        m_physicalDevice            = std::exchange(other.m_physicalDevice, nullptr);
        m_device                    = std::exchange(other.m_device, nullptr);
        m_queues                    = std::exchange(other.m_queues, { nullptr });
        m_swapChainSupportDetails   = std::exchange(other.m_swapChainSupportDetails, {});
        m_initialized               = std::exchange(other.m_initialized, false);

        return *this;
    };

    Device(const Device& other) noexcept = delete;
    Device& operator=(const Device& other) noexcept = delete;

    ~Device() { assert(!m_initialized && "Device was not destroyed!"); };

    void destroy(const vk::detail::DispatchLoaderDynamic& dispatcher) {
        if (!m_initialized)
            return;
        if (m_device) {
            m_device.destroy(nullptr, dispatcher);
        }
#ifdef _DEBUG
        std::cout << "Destroyed Device" << std::endl;
#endif
        m_initialized = false;
    }

    // Basic Device Access
    const vk::Device& getDevice() const  { return m_device; }
    vk::PhysicalDevice getPhysicalDevice() const { return m_physicalDevice; }

    // Device Properties/Capabilities
    vk::PhysicalDeviceProperties getProperties() const {
        return m_physicalDevice.getProperties();
    };

    vk::PhysicalDeviceFeatures getFeatures() const {
        return m_physicalDevice.getFeatures();
    };

    vk::PhysicalDeviceMemoryProperties getMemoryProperties() const {
        return m_physicalDevice.getMemoryProperties();
    };

    SwapChainSupportDetails getSwapChainSupportDetails() const {
        return m_swapChainSupportDetails;
    };

    bool reevaluateSwapChainSupportDetails(const EngineContext& instance, const Window& window) {
        auto result = m_physicalDevice.getSurfaceCapabilitiesKHR(window.getSurface(), &m_swapChainSupportDetails.capabilities, instance.getDispatchLoader());
        if (result != vk::Result::eSuccess) {
            throw std::runtime_error("failed to get surface capabilities!");
        }
        m_swapChainSupportDetails.formats = m_physicalDevice.getSurfaceFormatsKHR(window.getSurface());
        m_swapChainSupportDetails.presentModes = m_physicalDevice.getSurfacePresentModesKHR(window.getSurface());

        if (m_swapChainSupportDetails.formats.empty() || m_swapChainSupportDetails.presentModes.empty())
            return false;
        return true;
    };

    BasicRenderingQueueFamilyIndices getBasicIndices() const {
        return m_basicIndices;
    }

    static bool evaluateDevice(vk::PhysicalDevice device, const EngineContext& instance, const Window& window,
        BasicRenderingQueueFamilyIndices& basicIndices, SwapChainSupportDetails& swapChainSupportDetails);

    QueueCreateData getQueueData(QueueSpecialisation specialisation) const {
        return { m_queues[static_cast<size_t>(QueueSpecialisation::GRAPHICS)],
        m_basicIndices.families[static_cast<size_t>(QueueSpecialisation::GRAPHICS)].value(),
        specialisation };
    };

    void waitIdle(const EngineContext& instance) const {
        m_device.waitIdle(instance.getDispatchLoader());
    };

    vk::DeviceMemory allocateMemory(const EngineContext& instance,
        size_t requiredCapacity, uint32_t memoryType) const;

    uint32_t findMemoryType(const EngineContext& instance, 
        uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;

    vk::Format findSupportedFormat(const EngineContext& instance, 
    const std::vector<vk::Format>& candidates,
        vk::ImageTiling tiling, vk::FormatFeatureFlags features) const;
private:
    void selectPhysicalDevice(const EngineContext& instance,
        const Window& window);
    void createLogicalDevice(const EngineContext& instance,
        const Window& window);
    
};

