#pragma once
#include "Common.h"
#include "ResourceManager.h"
#include "Rendering/Device.h"
#include "Rendering/SwapChain.h"
#include "Rendering/RenderRegion.h"
#include "Rendering/RenderPass.h"
#include "Rendering/GraphicsPipeline.h"
#include "Rendering/SwapChainFormat.h"
#include "Rendering/CommandPool.h"
#include "Rendering/CommandBuffer.h"
#include "Rendering/Semaphore.h"
#include "Rendering/Fence.h"
#include "Rendering/Queue.h"
#include "BufferDataLayouts.h"
#include "MemoryManagement/Memory.h"
#include "MemoryManagement/Buffer.h"
#include "MemoryManagement/MappedMemory.h"
#include "MemoryManagement/DescriptorPool.h"
#include "MemoryManagement/DescriptorSet.h"
#include "Camera.h"
#include "PlatformManagement/Mouse.h"
#include "PlatformManagement/Keyboard.h"

class Engine
{
	struct FrameRenderObjects
	{
		CommandBufferHandle graphicsCommandBuffer;
		Semaphore imageAvailableSemaphore;
		Semaphore renderFinishedSemaphore;
		Fence inFlightFence;
		Buffer uniformBuffer;		
		DescriptorSetHandle perFrameSet;
		DescriptorSetHandle storageSet;
	};

private:
	const int m_width = 800;
	const int m_height = 600;

	int m_maxFramesInFlight = 2; //values above 3 might cause issues
	uint32_t m_currentFrame = 0;
	float m_deltaTime = 0.0f;

	bool m_isInitialized{ false };
	int m_frameNumber{ 0 };
	bool m_shouldRender{ false };
	
	ResourceManager m_resourceManager;

	GraphicsContext m_context;
	PhysicalDeviceCache m_deviceCache;
	Device m_device;
	Window m_window;
	RenderRegion m_canvas;
	SwapChainFormat m_format;
	SwapChain m_swapChain;
	RenderPass m_renderPass;
	GraphicsPipeline m_pipeline;
	CommandPool m_graphicsCommandPool;
	CommandPool m_temporaryBufferPool;
	DescriptorPool m_descriptorPool;

	Queue m_graphicsQueue;
	Queue m_presentQueue;
	Queue m_transferQueue;

	Shader m_vertex, m_fragment;

	DescriptorSetLayout m_perFrameLayout;
	DescriptorSetLayout m_storageLayout;

	std::vector<FrameRenderObjects> m_frameRenderObjects;

	MappedMemory m_stagingMemory;

	Memory m_positionMemory;
	Memory m_uvMemory;
	Memory m_modelTransformMemory;
	Memory m_textureIdMemory;
	Memory m_indexMemory;

	std::vector<Memory> m_imageMemories;

	MappedMemory m_uniformMemory;

	Buffer m_stagingBuffer;
	Buffer m_positionBuffer;
	Buffer m_uvBuffer;
	Buffer m_modelTransformBuffer;
	Buffer m_textureIdBuffer;
	Buffer m_indexBuffer;
	std::vector<Image> m_imageBuffers;
	Sampler m_sampler;

	UniformTransforms m_transforms;
	Camera<ProjectionType::PERSPECTIVE> m_camera;

	Mouse m_mouse;
	Keyboard m_keyboard;

	float m_mouseSensitivity = 10.f;
	float m_moveSpeed = 3.f;

	Window::WindowEventSubscription m_frameBufferResizeSubscription;

	static inline const std::array<std::string, 6> texturePaths = {
		"textures\\dirt.png",
		"textures\\grass_block_side.png",
		"textures\\grass_block_top.png",
		"textures\\stone.png",
		"textures\\water.png",
		"textures\\highlight_texture.png"
	};

public:
	Engine() = default;
	~Engine() { cleanup(); };

	//initializes everything in the engine
	void init(std::string engineName, std::string appName, Extent windowExtent);

	//shuts down the engine
	void cleanup();

	//draw loop, runs separately from the main loop
	void updateUniform();
	void drawFrame();
	void handleResize();
	void handleInputs();
	//run main loop
	void run();

	inline std::string getName() const { return m_context.getEngineName(); };
};
