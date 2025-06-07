#pragma once
#include "Common.h"
#include "Flags.h"
#include "PlatformManagement/PlatformContext.h"

class EngineContext
{
private:
    std::string m_engineName;
    std::string m_appName;
    vk::Instance m_instance;
    EngineContext** m_enginePointer = nullptr;
    mutable vk::detail::DispatchLoaderDynamic m_dispatchLoader;

    bool m_initialized = false;

    static inline std::set<std::string> activeEngines;

    //DebugRelated
#ifdef _DEBUG
    DebugMessageSeverity::Flags m_debugMessageSeveritySettings;
    vk::DebugUtilsMessengerEXT m_debugMessenger;
#endif

public:
    static inline const std::vector<const char*> validationLayers = {
#ifdef _DEBUG
    "VK_LAYER_KHRONOS_validation"
#endif
    };

#ifdef _DEBUG
    static inline const bool enabledValidationLayers = true;
#else
    static inline const bool enabledValidationLayers = false;
#endif

public:

    EngineContext() : m_instance(nullptr) 
#ifdef _DEBUG
    ,  m_debugMessenger(nullptr)
#endif
    {};

    EngineContext(const std::string& engineName, const std::string& appName,
        Version engineVersion, Version appVersion,
        DebugMessageSeverity::Flags debugMessageSeveritySettings = DebugMessageSeverity::Bits::ALL);

    EngineContext(EngineContext&& other) noexcept
    {
        m_engineName = std::exchange(other.m_engineName, std::string());
        m_appName = std::exchange(other.m_appName, std::string());
        m_enginePointer = std::exchange(other.m_enginePointer, nullptr);
        *m_enginePointer = this;
        m_instance = std::exchange(other.m_instance, vk::Instance());
#ifdef _DEBUG
        m_debugMessenger = std::exchange(other.m_debugMessenger, vk::DebugUtilsMessengerEXT());
        m_debugMessageSeveritySettings = std::exchange(other.m_debugMessageSeveritySettings, DebugMessageSeverity::Bits::ALL);
#endif
        m_dispatchLoader = std::exchange(other.m_dispatchLoader, vk::detail::DispatchLoaderDynamic());
        m_initialized = std::exchange(other.m_initialized, false);
    };

    EngineContext& operator=(EngineContext&& other) noexcept
    {
        if (&other == this) {
            return *this;
        }

        assert(!m_initialized && "EngineContext::operator=() - EngineContext already initialized");

        m_engineName = std::exchange(other.m_engineName, std::string());
        m_appName = std::exchange(other.m_appName, std::string());
        m_enginePointer = std::exchange(other.m_enginePointer, nullptr);
        *m_enginePointer = this;
        m_instance = std::exchange(other.m_instance, vk::Instance());
#ifdef _DEBUG
        m_debugMessenger = std::exchange(other.m_debugMessenger, vk::DebugUtilsMessengerEXT());
        m_debugMessageSeveritySettings = std::exchange(other.m_debugMessageSeveritySettings, DebugMessageSeverity::Bits::ALL);
#endif
        m_dispatchLoader = std::exchange(other.m_dispatchLoader, vk::detail::DispatchLoaderDynamic());
        m_initialized = std::exchange(other.m_initialized, false);

        return *this;
    };

    EngineContext(const EngineContext& other) noexcept = delete;
    EngineContext& operator=(const EngineContext& other) noexcept = delete;

    ~EngineContext() { assert(!m_initialized && "EngineContext was not destroyed!"); };

    void destroy();

    const std::string& getEngineName() const { return m_engineName; };
    const std::string& getAppName() const { return m_appName; };
    const vk::DebugUtilsMessengerEXT& getDebugManager() const { 
#ifdef _DEBUG
        return m_debugMessenger;
#else
        return vk::DebugUtilsMessengerEXT();
#endif
    };
    const vk::detail::DispatchLoaderDynamic& getDispatchLoader() const { return m_dispatchLoader; };
    const vk::Instance& getInstance() const { return m_instance; };

    void updateDispatchLoader(const vk::Device& device) const { m_dispatchLoader.init(m_instance, device); };

    static VKAPI_ATTR VkBool32 VKAPI_CALL static_debugCallback(
        vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        vk::DebugUtilsMessageTypeFlagsEXT messageType,
        const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        vk::DebugUtilsMessageTypeFlagsEXT messageType,
        const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    void setDebugSeverityThreshold(DebugMessageSeverity::Flags severitySettings);

private:

    static std::vector<vk::LayerProperties> getAvailableValidationLayers();

    static std::vector<const char*> validateLayers(std::vector<vk::LayerProperties>& availible);

    static void checkLayerSupport();

    static std::vector<const char*> getRequiredExtensions();

    static std::vector<vk::ExtensionProperties> getAvailableExtensions();

    static void validateExtensions(
        std::vector<vk::ExtensionProperties>& available,
        std::vector<const char*>& required);

    static vk::ApplicationInfo getAppInfo(const std::string& engineName,
        const std::string& appName, Version engineVersion, Version appVersion);

    static vk::InstanceCreateInfo getInstanceCreateInfo(vk::ApplicationInfo& appInfo,
        std::vector<const char*>& requiredExtensions,
        vk::DebugUtilsMessengerCreateInfoEXT* debugCreateInfo = nullptr);

    static vk::Instance getInstance(const std::string& engineName,
        const std::string& appName, Version& engineVersion, Version& appVersion,
        vk::DebugUtilsMessengerCreateInfoEXT* debugCreateInfo = nullptr);

    static vk::detail::DispatchLoaderDynamic getDispatchLoader(
        const vk::Instance& instance,
        const vk::Device& device = nullptr);

    static vk::DebugUtilsMessengerCreateInfoEXT getDebugCreateInfo(
        vk::PFN_DebugUtilsMessengerCallbackEXT callback,
        void* pUserData = nullptr);

    static vk::DebugUtilsMessengerEXT getDebugMessenger(
        vk::detail::DispatchLoaderDynamic& dispatchLoader, vk::Instance& instance,
        vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo);
};

