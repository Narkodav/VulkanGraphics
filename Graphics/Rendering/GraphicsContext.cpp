#include "GraphicsContext.h"

std::vector<vk::LayerProperties> GraphicsContext::getAvailableValidationLayers() {
    try {
        uint32_t layerCount;
        vk::enumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<vk::LayerProperties> availableLayers(layerCount);
        vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        return availableLayers;
    }
    catch (const vk::SystemError& e) {
        std::cerr << "Failed to enumerate instance layer properties: " << e.what() << std::endl;
        throw std::runtime_error(std::string("Failed to enumerate instance layer properties: ") + e.what());
    }
};

std::vector<const char*> GraphicsContext::validateLayers(std::vector<vk::LayerProperties>& availible)
{
    std::vector<const char*> unavailible;

    for (auto& layer : validationLayers) {
        bool found = false;
        for (auto& availibleLayer : availible) {
            if (strcmp(layer, availibleLayer.layerName) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            unavailible.push_back(layer);
        }
    }
    return unavailible;
};

void GraphicsContext::checkLayerSupport()
{
    std::vector<vk::LayerProperties> availible = getAvailableValidationLayers();
    auto unavailible = validateLayers(availible);

    std::cout << std::endl << "Available layers:" << std::endl;
    for (auto extension : availible)
        std::cout << "\t" << extension.layerName << std::endl;

    std::cout << std::endl << "Required layers:" << std::endl;
    for (auto extension : validationLayers)
        std::cout << "\t" << extension << std::endl;

    if (unavailible.size() > 0)
    {
        std::cout << std::endl << "Unavailable layers:" << std::endl;
        for (auto layer : unavailible)
            std::cout << "\t" << layer << std::endl;
        throw std::runtime_error("Unavailable layers");
    }
};

std::vector<const char*> GraphicsContext::getRequiredExtensions()
{
    try {
        uint32_t RequiredExtensionCount = 0;
        std::vector<const char*> RequiredExtensions;
        const char** buffer;
        buffer = glfwGetRequiredInstanceExtensions(&RequiredExtensionCount);
        RequiredExtensions.assign(buffer, buffer + RequiredExtensionCount);
#ifdef _DEBUG
        RequiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);    
#endif
        return RequiredExtensions;
    }
    catch (const vk::SystemError& e) {
        std::cerr << "Failed to enumerate required extensions: " << e.what() << std::endl;
        throw std::runtime_error(std::string("Failed to enumerate required extensions: ") + e.what());
    }
};

std::vector<vk::ExtensionProperties> GraphicsContext::getAvailableExtensions()
{
    try {
        uint32_t extensionCount = 0;
        vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<vk::ExtensionProperties> extensions(extensionCount);
        vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
        return extensions;
    }
    catch (const vk::SystemError& e) {
        std::cerr << "Failed to enumerate instance extensions: " << e.what() << std::endl;
        throw std::runtime_error(std::string("Failed to enumerate instance extensions: ") + e.what());
    }
};

void GraphicsContext::validateExtensions(
    std::vector<vk::ExtensionProperties>& available, std::vector<const char*>& required)
{
    std::vector<const char*> unavailible;

    for (auto& requiredExtension : required) {
        bool found = false;
        for (auto& availibleExtension : available) {
            if (strcmp(requiredExtension, availibleExtension.extensionName) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            unavailible.push_back(requiredExtension);
        }
    }

    std::cout << std::endl << "Available extensions:" << std::endl;
    for (auto extension : available)
        std::cout << "\t" << extension.extensionName << std::endl;

    std::cout << std::endl << "Required extensions:" << std::endl;
    for (auto extension : required)
        std::cout << "\t" << extension << std::endl;

    if (unavailible.size() > 0)
    {
        std::cout << std::endl << "Unavailable extensions:" << std::endl;
        for (auto extension : unavailible)
            std::cout << "\t" << extension << std::endl;
        throw std::runtime_error("Unavailable extensions");
    }
};

vk::ApplicationInfo GraphicsContext::getAppInfo(const std::string& engineName,
    const std::string& appName, Version engineVersion, Version appVersion) {
    vk::ApplicationInfo appInfo{};
    appInfo.sType = vk::StructureType::eApplicationInfo;
    appInfo.pEngineName = engineName.c_str();
    appInfo.pApplicationName = appName.c_str();
    appInfo.apiVersion = VK_API_VERSION_1_3;
    appInfo.applicationVersion = appVersion.getVersion();
    appInfo.engineVersion = engineVersion.getVersion();
    appInfo.pNext = nullptr;
    return appInfo;
};

vk::InstanceCreateInfo GraphicsContext::getInstanceCreateInfo(vk::ApplicationInfo& appInfo,
    std::vector<const char*>& requiredExtensions,
    vk::DebugUtilsMessengerCreateInfoEXT* debugCreateInfo /*= nullptr*/)
{
    vk::InstanceCreateInfo createInfo{};
    createInfo.sType = vk::StructureType::eInstanceCreateInfo;
    createInfo.pApplicationInfo = &appInfo;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();


#ifdef _DEBUG
    checkLayerSupport();
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
    createInfo.pNext = debugCreateInfo;
#else
    createInfo.enabledLayerCount = 0;
#endif
    return createInfo;
};

vk::Instance GraphicsContext::getInstance(const std::string& engineName,
    const std::string& appName, Version& engineVersion, Version& appVersion,
    vk::DebugUtilsMessengerCreateInfoEXT* debugCreateInfo /*= nullptr*/)
{
    vk::Instance instance;
    vk::ApplicationInfo appInfo = getAppInfo(
        engineName, appName, engineVersion, appVersion);

    std::vector<vk::ExtensionProperties> available = getAvailableExtensions();
    std::vector<const char*> required = getRequiredExtensions();
    validateExtensions(available, required);
    vk::InstanceCreateInfo createInfo = getInstanceCreateInfo(
        appInfo, required, debugCreateInfo);

    if (vk::createInstance(&createInfo, nullptr, &instance) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create vulkan instance");
    }

    return instance;
};

vk::detail::DispatchLoaderDynamic GraphicsContext::getDispatchLoader(
    const vk::Instance& instance,
    const vk::Device& device /*= nullptr*/)
{
    try {
        vk::detail::DispatchLoaderDynamic dispatcher(instance, vkGetInstanceProcAddr);
        // If device is provided, initialize device-specific functions
        if (device) {
            dispatcher.init(instance, device);
        }

        return dispatcher;
    }
    catch (const vk::SystemError& e) {
        throw std::runtime_error(std::string("Failed to create dispatch loader: ") + e.what());
    }
};

vk::DebugUtilsMessengerCreateInfoEXT GraphicsContext::getDebugCreateInfo(
    vk::PFN_DebugUtilsMessengerCallbackEXT callback,
    void* pUserData /*= nullptr*/)
{
    vk::DebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

    createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
    
    createInfo.pfnUserCallback = callback;
    createInfo.pUserData = pUserData; // Optional
    return createInfo;
};


vk::DebugUtilsMessengerEXT GraphicsContext::getDebugMessenger(
    vk::detail::DispatchLoaderDynamic& dispatchLoader, vk::Instance& instance,
    vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo)
{
    vk::DebugUtilsMessengerEXT debugMessenger =
        instance.createDebugUtilsMessengerEXT(debugCreateInfo, nullptr, dispatchLoader);
    return debugMessenger;
};

GraphicsContext::GraphicsContext(const std::string& engineName, const std::string& appName,
    Version engineVersion, Version appVersion,
    DebugMessageSeverity::Flags debugMessageSeveritySettings /*= DebugMessageSeverity::Bits::ALL*/)
#ifdef _DEBUG
    : m_debugMessageSeveritySettings(debugMessageSeveritySettings)
#endif
{
    PlatformContext::instance();
    activeEngines.insert(engineName);

    m_appName = appName;
    m_engineName = engineName;
    m_enginePointer = new GraphicsContext*;
    *m_enginePointer = this;
    
#ifdef _DEBUG
    vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo =
        getDebugCreateInfo(static_debugCallback, m_enginePointer);
    m_instance = getInstance(engineName, appName,
        engineVersion, appVersion, &debugCreateInfo);
    m_dispatchLoader = getDispatchLoader(m_instance);

    m_debugMessenger = getDebugMessenger(m_dispatchLoader,
        m_instance, debugCreateInfo);
#else
    (void)debugMessageSeveritySettings;
    m_instance = getInstance(engineName, appName,
        engineVersion, appVersion);
    m_dispatchLoader = getDispatchLoader(m_instance);
#endif
    m_initialized = true;
}

void GraphicsContext::destroy()
{
    if (!m_initialized)
        return;
#ifdef _DEBUG
    m_instance.destroyDebugUtilsMessengerEXT(
        m_debugMessenger, nullptr, m_dispatchLoader);
    m_debugMessenger = VK_NULL_HANDLE;
#endif
    m_instance.destroy(nullptr, m_dispatchLoader);

    activeEngines.erase(m_engineName);
#ifdef _DEBUG
    std::cout << "Destroyed GraphicsContext" << std::endl;
#endif
    delete m_enginePointer;
    m_initialized = false;
}

void GraphicsContext::setDebugSeverityThreshold(DebugMessageSeverity::Flags severitySettings)
{
#ifdef _DEBUG
    m_debugMessageSeveritySettings = severitySettings;
#else
    (void)severitySettings;
#endif
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL GraphicsContext::static_debugCallback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    vk::DebugUtilsMessageTypeFlagsEXT messageType,
    const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    GraphicsContext* context = *static_cast<GraphicsContext**>(pUserData);
    return context->debugCallback(messageSeverity, messageType,
        pCallbackData, pUserData);
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL GraphicsContext::debugCallback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    vk::DebugUtilsMessageTypeFlagsEXT messageType,
    const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
#ifdef _DEBUG
    if ((static_cast<uint32_t>(m_debugMessageSeveritySettings) &
        static_cast<uint32_t>(messageSeverity)) == 0)
        return VK_FALSE;
#endif
    std::cerr << "Engine [" << m_engineName << "], Application [" << m_appName << "] ";
    switch (messageSeverity) {
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
        // Diagnostic messages
        std::cout << "VERBOSE: " << pCallbackData->pMessage << std::endl;
        break;

    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
        // Informational messages like resource creation
        std::cout << "INFO: " << pCallbackData->pMessage << std::endl;
        break;

    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
        // Warning messages like use of deprecated functions
        std::cout << "WARNING: " << pCallbackData->pMessage << std::endl;
        break;

    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
        // Error messages for invalid behavior
        std::cerr << "ERROR: " << pCallbackData->pMessage << std::endl;
        throw std::runtime_error("Validation layer error");
        break;

    default:
        std::cout << "UNKNOWN SEVERITY: " << pCallbackData->pMessage << std::endl;
        std::abort();
        break;
    }
    return VK_FALSE;
}