#include "Engine.h"

void Engine::init(std::string engineName, std::string appName, Extent windowExtent)
{
    m_context = GraphicsContext(engineName, appName,
        Version(1, 0, 0), Version(1, 0, 0), DebugMessageSeverity::Bits::WARNING
        | DebugMessageSeverity::Bits::ERROR);
    m_resourceManager.registerResource(&m_context, "m_context",
        [this]() {m_context.destroy(); },
        //{},
        __FILE__, __LINE__);

    m_deviceCache = PhysicalDeviceCache(m_context);  

    m_window = Window(m_context, windowExtent,
        appName, Window::Attributes::firstPersonGameAtr());
    m_resourceManager.registerResource(&m_window, "m_window",
        [this]() {m_window.destroy(m_context); },
        //{ "m_context" },
        __FILE__, __LINE__);

    m_mouse.init(m_window);
    m_keyboard.init(m_window);

    m_frameBufferResizeSubscription = m_window.registerCallback<WindowEvents::FRAME_BUFFER_RESIZED>(
        [this](int width, int height) {
            handleResize();
        });

    DeviceRequirements requirements;
    requirements.extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        /*VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME*/ };
    requirements.properties = { {DeviceProperty::DEVICE_TYPE, PhysicalDeviceType::DISCRETE_GPU} };
    requirements.features = { {DeviceFeature::GEOMETRY_SHADER, true},
    {DeviceFeature::SAMPLER_ANISOTROPY, true},
    {DeviceFeature::DESCRIPTOR_BINDING_PARTIALLY_BOUND, true},
    {DeviceFeature::RUNTIME_DESCRIPTOR_ARRAY, true},
    {DeviceFeature::SHADER_SAMPLED_IMAGE_ARRAY_NON_UNIFORM_INDEXING, true},
    {DeviceFeature::DESCRIPTOR_BINDING_SAMPLED_IMAGE_UPDATE_AFTER_BIND, true},
    {DeviceFeature::DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT, true} };

    requirements.queueProperties.push_back(RequiredQueueProperties());
    requirements.queueProperties.back().queueProperties
        .insert({ QueueProperty::QUEUE_FLAGS, QueueFlags::Flags(QueueFlags::Bits::GRAPHICS | QueueFlags::Bits::TRANSFER) });
    requirements.queueProperties.back().shouldSupportPresent = false;

    requirements.queueProperties.push_back(RequiredQueueProperties());
    requirements.queueProperties.back().shouldSupportPresent = true;

    auto result = m_deviceCache.getFittingDevice(m_context, m_window, requirements);
    if (!result.isSuitable())
        std::cout << "No suitable device found" << std::endl;
    else std::cout << "Suitable device found" << std::endl;

    //search for discrete graphics, present and transfer families
    uint32_t graphicsIndex, presentIndex;

    graphicsIndex = result.queueFamilyIndices[0].front();
    size_t i;

    for (i = 0; i < result.queueFamilyIndices[1].size(); i++)
        if (graphicsIndex != result.queueFamilyIndices[1][i])
        {
            presentIndex = result.queueFamilyIndices[1][i];
            break;
        }
    if (i == result.queueFamilyIndices[1].size()) {
        presentIndex = result.queueFamilyIndices[1].front();
    }

    m_device = Device(m_context, *result.device, requirements,
        std::vector<uint32_t>{ graphicsIndex, presentIndex },
        std::vector<uint32_t>{ 2, 1 },
        std::vector<std::vector<float>>({ {1.0f,1.0f},{1.0f} }));
    m_resourceManager.registerResource(&m_device, "m_device",
        [this]() {m_device.destroy(m_context); },
        //{ "m_context" },
        __FILE__, __LINE__);

    m_graphicsQueue = Queue(m_context, m_device, graphicsIndex, 0);
    m_presentQueue = Queue(m_context, m_device, presentIndex, 0);
    m_transferQueue = Queue(m_context, m_device, graphicsIndex, 1);

    //canvas just stores dimensions, so it doesn't need to have a dedicated delete function
    //but it still has to be updated at window resize
    m_canvas = RenderRegion::createFullWindow(m_window);
    m_format = SwapChainFormat::create(m_context, m_device, m_window);

    m_renderPass = RenderPass(m_context, m_device, m_format);
    m_resourceManager.registerResource(&m_renderPass, "m_renderPass",
        [this]() {m_renderPass.destroy(m_context, m_device); },
        //{ "m_device" },
        __FILE__, __LINE__);

    m_swapChain = SwapChain(m_context, m_device, m_window,
        m_renderPass, m_format, presentIndex, graphicsIndex);

    m_resourceManager.registerResource(&m_swapChain, "m_swapChain",
        [this]() {m_swapChain.destroy(m_context, m_device); },
        //{ "m_renderPass", "m_window" },
        __FILE__, __LINE__);

    m_vertex = Shader(m_context, m_device, "Shaders/vert.spv");
    m_fragment = Shader(m_context, m_device, "Shaders/frag.spv");

    m_resourceManager.registerResource(&m_vertex, "m_vertex",
        [this]() {m_vertex.destroy(m_context, m_device); },
        //{ "m_device" },
        __FILE__, __LINE__);

    m_resourceManager.registerResource(&m_fragment, "m_fragment",
        [this]() {m_fragment.destroy(m_context, m_device); },
        //{ "m_device" },
        __FILE__, __LINE__);

    //DescriptorSetLayout(const GraphicsContext & instance, const Device & device,
    //    const DescriptorDefinitions<DescriptorDefs...>&descriptors,
    //    DescriptorSetLayoutCreate::Flags flags = DescriptorSetLayoutCreate::Bits::NONE)

    m_perFrameLayout = DescriptorSetLayout(m_context, m_device,
        DescriptorDefinitions<UniformTransformsDefinition>());
    m_resourceManager.registerResource(&m_storageLayout, "m_perFrameLayout",
        [this]() {m_perFrameLayout.destroy(m_context, m_device); },
        //{ "m_swapChain" },
        __FILE__, __LINE__);

    m_storageLayout = DescriptorSetLayout(m_context, m_device,
        DescriptorDefinitions<BindlessImageSamplerDefinition<0>>(),
        DescriptorSetLayoutCreate::Bits::UPDATE_AFTER_BIND);
    m_resourceManager.registerResource(&m_storageLayout, "m_storageLayout",
        [this]() {m_storageLayout.destroy(m_context, m_device); },
        //{ "m_swapChain" },
        __FILE__, __LINE__);

    m_pipeline = GraphicsPipeline(m_context, m_device, m_renderPass,
        { m_vertex, m_fragment }, m_canvas, m_format,
        VertexDefinitions<VertexDefinitionPosition,
        VertexDefinitionColor, VertexDefinitionModelTransform, VertexDefinitionId>(),
        { &m_perFrameLayout, &m_storageLayout });

    m_resourceManager.registerResource(&m_pipeline, "m_pipeline",
        [this]() {m_pipeline.destroy(m_context, m_device); },
        //{ "m_swapChain" },
        __FILE__, __LINE__);

    m_graphicsCommandPool = CommandPool(m_context, m_device, graphicsIndex);
    m_resourceManager.registerResource(&m_graphicsCommandPool, "m_graphicsCommandPool",
        [this]() {
            m_graphicsCommandPool.reset(m_context, m_device);
            m_graphicsCommandPool.destroy(m_context, m_device);
        },
        //{ "m_device" },
        __FILE__, __LINE__);

    m_temporaryBufferPool = CommandPool(m_context, m_device, graphicsIndex);
    m_resourceManager.registerResource(&m_temporaryBufferPool, "m_temporaryBufferPool",
        [this]() {
            m_temporaryBufferPool.reset(m_context, m_device);
            m_temporaryBufferPool.destroy(m_context, m_device);
        },
        //{ "m_device" },
        __FILE__, __LINE__);

    m_descriptorPool = DescriptorPool(m_context, m_device, {
            DescriptorPool::Size(m_maxFramesInFlight, DescriptorType::UNIFORM_BUFFER),
            DescriptorPool::Size(
                m_maxFramesInFlight * 1024, DescriptorType::COMBINED_IMAGE_SAMPLER)
        }, m_maxFramesInFlight * 2,
        DescriptorPoolCreateFlags::Bits::UPDATE_AFTER_BIND);       

    m_resourceManager.registerResource(&m_descriptorPool, "m_descriptorPool",
        [this]() {
            m_descriptorPool.destroy(m_context, m_device);
        },
        //{ "m_device" },
        __FILE__, __LINE__);

    std::vector<DescriptorSetHandle> sets;
    std::vector<DescriptorSetHandle> setsVariableSized;
    {
        std::vector<const DescriptorSetLayout*> layouts;
        std::vector<const DescriptorSetLayout*> layoutsVariableSized;
        std::vector<uint32_t> descriptorCounts;

        for (size_t i = 0; i < m_maxFramesInFlight; ++i)
            layouts.push_back(&m_perFrameLayout);
        for (size_t i = 0; i < m_maxFramesInFlight; ++i)
        {
            layoutsVariableSized.push_back(&m_storageLayout);
            descriptorCounts.push_back(1024);
        }

        sets = m_descriptorPool.allocateSets(m_context, m_device, layouts);
        setsVariableSized = m_descriptorPool.allocateSets(m_context, m_device,
            layoutsVariableSized, descriptorCounts);
    }

    for (int i = 0; i < m_maxFramesInFlight; ++i)
    {
        m_frameRenderObjects.push_back({
        m_graphicsCommandPool.allocateBuffer(m_context, m_device),
        Semaphore(m_context, m_device),
        Semaphore(m_context, m_device),
        Fence(m_context, m_device, true),
        Buffer(m_context, m_device,
        Buffer::Descriptor(sizeof(UniformTransforms),
            BufferUsage::Bits::UNIFORM, false))
            });

        std::string name = "m_frameRenderObjects[" + std::to_string(i);
        name += "]";
        m_resourceManager.registerResource(&m_frameRenderObjects[i], name,
            [this, i]() {
                m_frameRenderObjects[i].imageAvailableSemaphore.destroy(m_context, m_device);
                m_frameRenderObjects[i].inFlightFence.destroy(m_context, m_device);
                m_frameRenderObjects[i].renderFinishedSemaphore.destroy(m_context, m_device);
                m_frameRenderObjects[i].uniformBuffer.destroy(m_context, m_device);
            },
            //{ "m_device" },
            __FILE__, __LINE__);
    }

    m_uniformMemory = MappedMemory(m_context, m_device,
        m_frameRenderObjects[0].uniformBuffer.getMemoryRequirements(),
        MemoryProperty::Bits::HOST_VISIBLE_COHERENT,
        sizeof(UniformTransforms) * m_maxFramesInFlight);
    m_resourceManager.registerResource(&m_uniformMemory, "m_uniformMemory",
        [this]() {
            m_uniformMemory.destroy(m_context, m_device);
        },
        //{ "m_device" },
        __FILE__, __LINE__);

    m_sampler = Sampler(m_context, m_device, Sampler::Descriptor(m_context, m_device));
    m_resourceManager.registerResource(&m_sampler, "m_sampler",
        [this]() {
            m_sampler.destroy(m_context, m_device);
        },
        //{ "m_device" },
        __FILE__, __LINE__);

    for (int i = 0; i < m_maxFramesInFlight; ++i)
    {
        m_uniformMemory.bindBuffer(m_context, m_device,
            m_frameRenderObjects[i].uniformBuffer, sizeof(UniformTransforms) * i);
        m_frameRenderObjects[i].perFrameSet = sets[i];
        m_frameRenderObjects[i].storageSet = setsVariableSized[i];
        m_frameRenderObjects[i].perFrameSet->write(m_context, m_device,
            m_frameRenderObjects[i].uniformBuffer, 0,
            0, sizeof(UniformTransforms));
    }

}

//shuts down the engine
void Engine::cleanup()
{
    //m_resourceManager.printDependencyGraph();
    m_resourceManager.cleanup();
}

void Engine::handleInputs()
{
    if (m_mouse.moved())
    {
        auto deltaPos = m_mouse.getMouseDeltaPos();
        m_camera.rotate(-deltaPos.y * m_deltaTime * m_mouseSensitivity, deltaPos.x * m_deltaTime * m_mouseSensitivity);
    }
    glm::vec3 moveDir(0.0f);
    if (m_keyboard.getInputState<KeyboardKey::W, KeyboardKeyState::PRESSED>())
        moveDir += m_camera.getCamForward();
    if (m_keyboard.getInputState<KeyboardKey::S, KeyboardKeyState::PRESSED>())
        moveDir -= m_camera.getCamForward();
    if (m_keyboard.getInputState<KeyboardKey::A, KeyboardKeyState::PRESSED>())
        moveDir -= m_camera.getCamRight();
    if (m_keyboard.getInputState<KeyboardKey::D, KeyboardKeyState::PRESSED>())
        moveDir += m_camera.getCamRight();
    if (m_keyboard.getInputState<KeyboardKey::SPACE, KeyboardKeyState::PRESSED>())
        moveDir += m_camera.getWorldUp();
    if (m_keyboard.getInputState<KeyboardKey::LSHIFT, KeyboardKeyState::PRESSED>())
        moveDir -= m_camera.getWorldUp();
    glm::normalize(moveDir);

    if (moveDir.length != 0)
        m_camera.move(moveDir * m_deltaTime * m_moveSpeed);

    m_keyboard.refreshState();
    m_mouse.refreshState();
}

void Engine::handleResize()
{
    if (m_window.isMinimised())
        return;
    m_device.waitIdle(m_context);
    m_canvas = RenderRegion::createFullWindow(m_window);
    m_format = SwapChainFormat::create(m_context, m_device, m_window);
    m_camera.setAspectRatio(m_window.getFrameBufferExtent().width /
        (float)m_window.getFrameBufferExtent().height);
    m_transforms.proj = m_camera.getProjection();
    m_swapChain.recreate(m_context, m_device, m_window, m_renderPass, m_format,
        m_presentQueue.getFamily(), m_graphicsQueue.getFamily());
}

void Engine::updateUniform()
{
    m_transforms.view = m_camera.getView();
    auto mapping = m_uniformMemory.getMapping<UniformTransforms>(m_maxFramesInFlight);
    mapping[m_currentFrame] = m_transforms;
}

//draw loop
void Engine::drawFrame()
{
    m_frameRenderObjects[m_currentFrame].inFlightFence.wait(m_context, m_device);

    uint32_t imageIndex; 
    if (!m_swapChain.acquireNextImage(m_context, m_device,
        m_frameRenderObjects[m_currentFrame].imageAvailableSemaphore, imageIndex))
    {
        handleResize();
        return;
    }

    m_frameRenderObjects[m_currentFrame].graphicsCommandBuffer->reset(m_context);
    m_frameRenderObjects[m_currentFrame].graphicsCommandBuffer->record(m_context);
    m_frameRenderObjects[m_currentFrame].graphicsCommandBuffer->beginRenderPass(m_context, m_renderPass,
        m_swapChain, imageIndex, Color::Green(), 1);
    m_frameRenderObjects[m_currentFrame].graphicsCommandBuffer->bindGraphicsPipeline(m_context, m_pipeline);
    m_frameRenderObjects[m_currentFrame].graphicsCommandBuffer->setRenderView(m_context, m_canvas);

    m_frameRenderObjects[m_currentFrame].graphicsCommandBuffer->bindVertexBuffers(m_context,
        std::array{ std::ref(m_positionBuffer), std::ref(m_uvBuffer),
        std::ref(m_modelTransformBuffer), std::ref(m_textureIdBuffer) },
        std::array{ vk::DeviceSize(0), vk::DeviceSize(0),
        vk::DeviceSize(0), vk::DeviceSize(0) });

    m_frameRenderObjects[m_currentFrame].graphicsCommandBuffer->bindIndexBuffer(m_context, m_indexBuffer, 0);
    m_frameRenderObjects[m_currentFrame].graphicsCommandBuffer->bindDescriptorSets(
        m_context, m_pipeline, { m_frameRenderObjects[m_currentFrame].perFrameSet,
            m_frameRenderObjects[m_currentFrame].storageSet }, {});

    m_frameRenderObjects[m_currentFrame].graphicsCommandBuffer->drawIndexed(m_context, 36, 9, 0, 0, 0);
    m_frameRenderObjects[m_currentFrame].graphicsCommandBuffer->endRenderPass(m_context);
    m_frameRenderObjects[m_currentFrame].graphicsCommandBuffer->stopRecord(m_context);
    m_frameRenderObjects[m_currentFrame].inFlightFence.reset(m_context, m_device);

    m_graphicsQueue.submit(
        m_context,
        std::array{ vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput) },
        std::array{ std::ref(m_frameRenderObjects[m_currentFrame].imageAvailableSemaphore) },
        std::array{ std::ref(m_frameRenderObjects[m_currentFrame].graphicsCommandBuffer) },
        std::array{ std::ref(m_frameRenderObjects[m_currentFrame].renderFinishedSemaphore) },
        m_frameRenderObjects[m_currentFrame].inFlightFence
    );
    
    if (!m_presentQueue.present(m_context,
        std::array{ std::ref(m_frameRenderObjects[m_currentFrame].renderFinishedSemaphore) },
        std::array{ std::ref(m_swapChain) },
        std::array{ imageIndex }))
    {
        handleResize();
        return;
    }

    m_currentFrame = (m_currentFrame + 1) % m_maxFramesInFlight;
}

//run main loop
void Engine::run()
{
    std::vector<glm::vec4> positions = {
        // Front face
        {-0.5f, -0.5f,  0.5f, 1.0f},  // 0
        { 0.5f, -0.5f,  0.5f, 1.0f},  // 1
        { 0.5f,  0.5f,  0.5f, 1.0f},  // 2
        {-0.5f,  0.5f,  0.5f, 1.0f},  // 3
        // Back face
        {-0.5f, -0.5f, -0.5f, 1.0f},  // 4
        { 0.5f, -0.5f, -0.5f, 1.0f},  // 5
        { 0.5f,  0.5f, -0.5f, 1.0f},  // 6
        {-0.5f,  0.5f, -0.5f, 1.0f}   // 7
    };

    std::vector<glm::vec2> uvs = {
        // Front face
        {0.0f, 0.0f},  // Bottom-left
        {1.0f, 0.0f},  // Bottom-right
        {1.0f, 1.0f},  // Top-right
        {0.0f, 1.0f},  // Top-left
        // Back face
        {1.0f, 0.0f},  // Bottom-right
        {0.0f, 0.0f},  // Bottom-left
        {0.0f, 1.0f},  // Top-left
        {1.0f, 1.0f}   // Top-right
    };

    std::vector<uint32_t> indices = {
        // Front face
        0, 1, 2,
        2, 3, 0,
        // Right face
        1, 5, 6,
        6, 2, 1,
        // Back face
        5, 4, 7,
        7, 6, 5,
        // Left face
        4, 0, 3,
        3, 7, 4,
        // Top face
        3, 2, 6,
        6, 7, 3,
        // Bottom face
        4, 5, 1,
        1, 0, 4
    };

    std::vector<glm::mat4> modelTransforms;

    modelTransforms.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 1.0f)));
    modelTransforms.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    modelTransforms.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, -1.0f)));

    modelTransforms.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f)));
    modelTransforms.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.0f)));
    modelTransforms.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, -1.0f)));

    modelTransforms.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, 1.0f)));
    modelTransforms.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, 0.0f)));
    modelTransforms.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, -1.0f)));

    std::vector<uint32_t> textureIds = { 0, 1, 2, 3, 4, 5, 0, 1, 2 };

    //std::vector<glm::vec4> colors = {
//    {1.0f, 0.0f, 0.0f, 1.0f},  // Red
//    {0.0f, 1.0f, 0.0f, 1.0f},  // Green
//    {0.0f, 0.0f, 1.0f, 1.0f},  // Blue
//    {1.0f, 1.0f, 0.0f, 1.0f},  // Yellow
//    {1.0f, 0.0f, 1.0f, 1.0f},  // Magenta
//    {0.0f, 1.0f, 1.0f, 1.0f},  // Cyan
//    {1.0f, 1.0f, 1.0f, 1.0f},  // White
//    {0.5f, 0.5f, 0.5f, 1.0f}   // Gray
//};

    m_camera = Camera<ProjectionType::PERSPECTIVE>(
        glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 2.0f),
        0.0f, -90.0f, 120,
        m_window.getFrameBufferExtent().width /
        (float)m_window.getFrameBufferExtent().height, 0.1f, 100.0f);

    m_transforms.view = m_camera.getView();
    m_transforms.proj = m_camera.getProjection();

    m_stagingBuffer = Buffer(m_context, m_device,
        static_cast<size_t>(1024 * 1024),
        BufferUsage::Bits::TRANSFER_SRC, false);

    m_stagingMemory = MappedMemory(m_context, m_device, m_stagingBuffer.getMemoryRequirements(),
        MemoryProperty::Bits::HOST_VISIBLE_COHERENT,
        static_cast<size_t>(1024 * 1024));

    m_positionBuffer = Buffer(m_context, m_device,
        Buffer::Descriptor(VertexDefinitionPosition::DATA_SIZE * positions.size(),
            BufferUsage::Bits::TRANSFER_DST | BufferUsage::Bits::VERTEX, false));

    m_positionMemory = Memory(m_context, m_device, m_positionBuffer.getMemoryRequirements(),
        MemoryProperty::Bits::DEVICE_LOCAL,
        VertexDefinitionPosition::DATA_SIZE * positions.size());

    m_uvBuffer = Buffer(m_context, m_device,
        Buffer::Descriptor(VertexDefinitionUV::DATA_SIZE * uvs.size(),
            BufferUsage::Bits::TRANSFER_DST | BufferUsage::Bits::VERTEX, false));

    m_uvMemory = Memory(m_context, m_device, m_uvBuffer.getMemoryRequirements(),
        MemoryProperty::Bits::DEVICE_LOCAL,
        VertexDefinitionUV::DATA_SIZE * uvs.size());

    m_modelTransformBuffer = Buffer(m_context, m_device,
        Buffer::Descriptor(VertexDefinitionModelTransform::DATA_SIZE * modelTransforms.size(),
            BufferUsage::Bits::TRANSFER_DST | BufferUsage::Bits::VERTEX, false));

    m_modelTransformMemory = Memory(m_context, m_device, m_modelTransformBuffer.getMemoryRequirements(),
        MemoryProperty::Bits::DEVICE_LOCAL,
        VertexDefinitionModelTransform::DATA_SIZE * modelTransforms.size());

    m_textureIdBuffer = Buffer(m_context, m_device,
        Buffer::Descriptor(VertexDefinitionId::DATA_SIZE * textureIds.size(),
            BufferUsage::Bits::TRANSFER_DST | BufferUsage::Bits::VERTEX, false));

    m_textureIdMemory = Memory(m_context, m_device, m_textureIdBuffer.getMemoryRequirements(),
        MemoryProperty::Bits::DEVICE_LOCAL,
        VertexDefinitionModelTransform::DATA_SIZE * textureIds.size());

    m_indexBuffer = Buffer(m_context, m_device,
        Buffer::Descriptor(sizeof(uint32_t) * indices.size(),
            BufferUsage::Bits::TRANSFER_DST | BufferUsage::Bits::INDEX, false));

    m_indexMemory = Memory(m_context, m_device, m_indexBuffer.getMemoryRequirements(),
        MemoryProperty::Bits::DEVICE_LOCAL,
        sizeof(uint32_t) * indices.size());

    size_t totalCapacity = 0;
    m_imageBuffers.resize(texturePaths.size());
    m_imageMemories.resize(texturePaths.size());

    for (size_t i = 0; i < texturePaths.size(); i++)
    {
        m_imageBuffers[i] = Image(m_context, m_device, texturePaths[i]);
        m_imageMemories[i] = Memory(m_context, m_device, m_imageBuffers[i].getMemoryRequirements(),
            MemoryProperty::Bits::DEVICE_LOCAL, m_imageBuffers[i].getMemoryRequirements().size);
        m_imageMemories[i].bindImage(m_context, m_device, m_imageBuffers[i]);
        m_imageBuffers[i].createView(m_context, m_device);
    }

    m_stagingMemory.bindBuffer(m_context, m_device, m_stagingBuffer);
    m_positionMemory.bindBuffer(m_context, m_device, m_positionBuffer);
    m_uvMemory.bindBuffer(m_context, m_device, m_uvBuffer);
    m_modelTransformMemory.bindBuffer(m_context, m_device, m_modelTransformBuffer);
    m_textureIdMemory.bindBuffer(m_context, m_device, m_textureIdBuffer);
    m_indexMemory.bindBuffer(m_context, m_device, m_indexBuffer);

    m_temporaryBufferPool.makeOneTimeSubmit(m_context, m_device, m_transferQueue,
        [this, &positions](CommandBufferHandle temporary) {
            auto mappedMemory = m_stagingMemory.getMapping<VertexDefinitionPosition::Type>(positions.size());
            std::copy(positions.begin(), positions.end(), mappedMemory.begin());
            temporary->transferBufferData(m_context, m_stagingBuffer,
                m_positionBuffer, CopyRegion(0, 0, positions.size() * VertexDefinitionPosition::DATA_SIZE));
        });

    m_temporaryBufferPool.makeOneTimeSubmit(m_context, m_device, m_transferQueue,
        [this, &uvs](CommandBufferHandle temporary) {
            auto mappedMemory = m_stagingMemory.getMapping<VertexDefinitionUV::Type>(uvs.size());
            std::copy(uvs.begin(), uvs.end(), mappedMemory.begin());
            temporary->transferBufferData(m_context, m_stagingBuffer,
                m_uvBuffer, CopyRegion(0, 0, uvs.size() * VertexDefinitionUV::DATA_SIZE));
        });

    m_temporaryBufferPool.makeOneTimeSubmit(m_context, m_device, m_transferQueue,
        [this, &modelTransforms](CommandBufferHandle temporary) {
            auto mappedMemory = m_stagingMemory.getMapping<VertexDefinitionModelTransform::Type>(modelTransforms.size());
            std::copy(modelTransforms.begin(), modelTransforms.end(), mappedMemory.begin());
            temporary->transferBufferData(m_context, m_stagingBuffer,
                m_modelTransformBuffer, CopyRegion(0, 0, modelTransforms.size() * VertexDefinitionModelTransform::DATA_SIZE));
        });

    m_temporaryBufferPool.makeOneTimeSubmit(m_context, m_device, m_transferQueue,
        [this, &textureIds](CommandBufferHandle temporary) {
            auto mappedMemory = m_stagingMemory.getMapping<VertexDefinitionId::Type>(textureIds.size());
            std::copy(textureIds.begin(), textureIds.end(), mappedMemory.begin());
            temporary->transferBufferData(m_context, m_stagingBuffer,
                m_textureIdBuffer, CopyRegion(0, 0, textureIds.size() * VertexDefinitionId::DATA_SIZE));
        });

    m_temporaryBufferPool.makeOneTimeSubmit(m_context, m_device, m_transferQueue,
        [this, &indices](const CommandBufferHandle& temporary) {
            auto mappedMemory = m_stagingMemory.getMapping<uint32_t>(indices.size());
            std::copy(indices.begin(), indices.end(), mappedMemory.begin());
            temporary->transferBufferData(m_context, m_stagingBuffer,
                m_indexBuffer, CopyRegion(0, 0, indices.size() * sizeof(uint32_t)));
        });

    for (size_t i = 0; i < m_imageBuffers.size(); i++)
    {
        m_temporaryBufferPool.makeOneTimeSubmit(m_context, m_device, m_transferQueue,
            [this, &i](CommandBufferHandle temporary) {
                temporary->setPipelineBarrier(m_context,
                    vk::PipelineStageFlagBits::eTopOfPipe,
                    vk::PipelineStageFlagBits::eTransfer,
                    m_imageBuffers[i],
                    vk::ImageLayout::eTransferDstOptimal,
                    vk::AccessFlagBits::eNone,
                    vk::AccessFlagBits::eTransferWrite
                    );

                auto pixelData = m_imageBuffers[i].getPixelData();
                auto mappedMemory = m_stagingMemory.getMapping<uint8_t>(pixelData.size());
                std::copy(pixelData.begin(), pixelData.end(), mappedMemory.begin());

                temporary->transferImageData(m_context, m_stagingBuffer, m_imageBuffers[i]);

                temporary->setPipelineBarrier(m_context,
                    vk::PipelineStageFlagBits::eTransfer,
                    vk::PipelineStageFlagBits::eFragmentShader,
                    m_imageBuffers[i],
                    vk::ImageLayout::eShaderReadOnlyOptimal,
                    vk::AccessFlagBits::eTransferWrite,
                    vk::AccessFlagBits::eShaderRead
                );
            });
    }

    std::vector<const Sampler*> samplerHandles(m_imageBuffers.size(), &m_sampler);

    for (int i = 0; i < m_maxFramesInFlight; ++i)
    {
        m_frameRenderObjects[i].storageSet->write(m_context, m_device,
            m_imageBuffers, samplerHandles, 0);
    }

    FrameRateCalculator calculator;
    calculator.setFrameTimeBuffer(100);

    while (!m_window.shouldClose()) {
        auto startTime = std::chrono::high_resolution_clock::now();

        try {
            m_window.pollEvents();
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }

        handleInputs();

        updateUniform();


        //game loop


        drawFrame();

        auto currentTime = std::chrono::high_resolution_clock::now();
        m_deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        calculator.addFrameTime(m_deltaTime);
        calculator.updateFrameRate();
        //calculator.displayFrameRateToConsole();
    }

    //wait for all graphics operations to finish
    m_device.waitIdle(m_context);

    m_stagingBuffer.destroy(m_context, m_device);
    m_stagingMemory.destroy(m_context, m_device);

    for (size_t i = 0; i < texturePaths.size(); i++)
    {
        m_imageBuffers[i].destroy(m_context, m_device);
        m_imageMemories[i].destroy(m_context, m_device);
    }

    m_positionBuffer.destroy(m_context, m_device);
    m_positionMemory.destroy(m_context, m_device);
    m_modelTransformBuffer.destroy(m_context, m_device);
    m_modelTransformMemory.destroy(m_context, m_device);
    m_uvBuffer.destroy(m_context, m_device);
    m_uvMemory.destroy(m_context, m_device);
    m_textureIdBuffer.destroy(m_context, m_device);
    m_textureIdMemory.destroy(m_context, m_device);
    m_indexBuffer.destroy(m_context, m_device);
    m_indexMemory.destroy(m_context, m_device);
}
