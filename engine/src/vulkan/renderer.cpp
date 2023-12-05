#include "arete/vulkan.hpp"

#include <chrono>

namespace vulkan
{

void VulkanRenderer::surface(GLFWwindow* window)
{
  // Create surface.
  VkSurfaceKHR directSurface;
  glfwCreateWindowSurface(*_instance, window, nullptr, &directSurface);
  _surface = vkr::SurfaceKHR(_instance, directSurface);

  // Swap chain extension for device.
  _devExtensions.emplace_back(
    VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

void VulkanRenderer::physicalDevice()
{
  // Query physical devices.
  const vkr::PhysicalDevices physicalDevices(_instance);
  // The first one should be good enough.
  _physicalDevice = physicalDevices.front();

  printf("Selected physical device: %s\n", _physicalDevice.getProperties().deviceName.data());
}

void VulkanRenderer::logicalDevice()
{
  float queuePriorities[] = {0.0f};

  const auto queueFamilyProperties = _physicalDevice.getQueueFamilyProperties();

  int32_t index = 0;
  // Find queue families for graphics & present
  for (const auto& queueFamily: queueFamilyProperties)
  {
    if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
    {
      _queueFamilyHints.graphicsFamily = index;

      const auto glfwSupport = glfwGetPhysicalDevicePresentationSupport(
        *_instance, *_physicalDevice, index);
      if (_physicalDevice.getSurfaceSupportKHR(index, *_surface) && glfwSupport == GLFW_TRUE)
      {
        _queueFamilyHints.presentFamily = index;
      }
      index++;
    }
  }

  if (!_queueFamilyHints.graphicsFamily || !_queueFamilyHints.presentFamily)
    throw std::runtime_error("No queue family supporting graphics and presentation found");

  // Device extensions in contiguous array.
  std::vector<const char*> extensions;
  extensions.reserve(_devExtensions.size());
  for (const auto& ext: _devExtensions)
  {
    extensions.emplace_back(ext.data());
  }

  const vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
    .queueFamilyIndex = _queueFamilyHints.graphicsFamily.value(),
    .queueCount = 1,
    .pQueuePriorities = queuePriorities,
  };

  // Create the device.
  _device = vkr::Device(
    _physicalDevice,
    vk::DeviceCreateInfo{
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &deviceQueueCreateInfo,
      .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
      .ppEnabledExtensionNames = extensions.data()});
}

void VulkanRenderer::swapChain()
{
  // Query the surface formats supported by the physical device.
  const auto surfaceFormats = _physicalDevice.getSurfaceFormatsKHR(*_surface);
  if (surfaceFormats.empty())
    throw std::runtime_error("No surface formats supported.");

  _surfaceImageFormat = surfaceFormats.front().format == vk::Format::eUndefined
                          ? vk::Format::eB8G8R8A8Unorm
                          : surfaceFormats.front().format;

  _surfaceCapabilities = _physicalDevice.getSurfaceCapabilitiesKHR(*_surface);

  const vk::Extent2D swapChainExtent =
    _surfaceCapabilities.currentExtent;

  const auto swapChainPresentMode = vk::PresentModeKHR::eFifo;

  const vk::SurfaceTransformFlagBitsKHR preTransform =
    _surfaceCapabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity
      ? vk::SurfaceTransformFlagBitsKHR::eIdentity
      : _surfaceCapabilities.currentTransform;

  const vk::CompositeAlphaFlagBitsKHR compositeAlpha =
    _surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied
      ? vk::CompositeAlphaFlagBitsKHR::ePreMultiplied
    : _surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied
      ? vk::CompositeAlphaFlagBitsKHR::ePostMultiplied
    : _surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit
      ? vk::CompositeAlphaFlagBitsKHR::eInherit
      : vk::CompositeAlphaFlagBitsKHR::eOpaque;

  vk::SwapchainCreateInfoKHR swapChainCreateInfo{
    .surface = *_surface,
    .minImageCount = _surfaceCapabilities.minImageCount,// buffering strategy
    .imageFormat = _surfaceImageFormat,
    .imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear,
    .imageExtent = swapChainExtent,
    .imageArrayLayers = 1,
    .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
    .imageSharingMode = vk::SharingMode::eExclusive,
    .preTransform = preTransform,
    .compositeAlpha = compositeAlpha,
    .presentMode = swapChainPresentMode,
    .clipped = true};

  const std::array queueFamilyIndices = {
    _queueFamilyHints.graphicsFamily.value(),
    _queueFamilyHints.presentFamily.value()};

  // If present family is different from graphics family,
  // enable image sharing across the family queues.
  if (_queueFamilyHints.graphicsFamily != _queueFamilyHints.presentFamily)
  {
    swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
    swapChainCreateInfo.queueFamilyIndexCount = 2;
    swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();
  }

  _swapChain = vkr::SwapchainKHR(
    _device, swapChainCreateInfo);

  const auto swapChainImages = _swapChain.getImages();
  _swapChainImageViews.reserve(swapChainImages.size());

  for (const auto& image: swapChainImages)
  {
    _swapChainImageViews.emplace_back(
      _device,
      vk::ImageViewCreateInfo{
        .image = image,
        .viewType = vk::ImageViewType::e2D,
        .format = _surfaceImageFormat,
        .subresourceRange = {
          .aspectMask = vk::ImageAspectFlagBits::eColor,
          .baseMipLevel = 0,
          .levelCount = 1,
          .baseArrayLayer = 0,
          .layerCount = 1,
        },
      });
  }
}

void VulkanRenderer::framebuffers()
{
  _framebuffers.reserve(_swapChainImageViews.size());

  for (const auto& swapChainImageView: _swapChainImageViews)
  {
    std::array framebufferAttachements = {
      *swapChainImageView,
      *_depthImageView,
    };

    _framebuffers.emplace_back(
      _device,
      vk::FramebufferCreateInfo{
        .renderPass = *_renderPass,
        .attachmentCount = framebufferAttachements.size(),
        .pAttachments = framebufferAttachements.data(),
        .width = _surfaceCapabilities.currentExtent.width,
        .height = _surfaceCapabilities.currentExtent.height,
        .layers = 1});
  }
}

void VulkanRenderer::depthBuffer()
{
  _depthImageFormat = vk::Format::eD16Unorm;
  const vk::FormatProperties formatProperties = _physicalDevice.getFormatProperties(_depthImageFormat);

  // Determine tiling available for depth buffer
  vk::ImageTiling imageTiling;
  if (formatProperties.linearTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
    imageTiling = vk::ImageTiling::eLinear;
  else if (formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
    imageTiling = vk::ImageTiling::eOptimal;
  else
    throw std::runtime_error("DepthStencilAttachment is not supported for D16Unorm depth format.");

  _depthImage = vkr::Image(
    _device,
    vk::ImageCreateInfo{
      .imageType = vk::ImageType::e2D,
      .format = _depthImageFormat,
      .extent = vk::Extent3D{
        .width = _surfaceCapabilities.currentExtent.width,
        .height = _surfaceCapabilities.currentExtent.height,
        .depth = 1},
      .mipLevels = 1,
      .arrayLayers = 1,
      .samples = vk::SampleCountFlagBits::e1,
      .tiling = imageTiling,
      .usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
      .sharingMode = vk::SharingMode::eExclusive,
    });

  const vk::PhysicalDeviceMemoryProperties memoryProperties = _physicalDevice.getMemoryProperties();
  const vk::MemoryRequirements memoryRequirements = _depthImage.getMemoryRequirements();

  uint32_t memoryTypeBits = memoryRequirements.memoryTypeBits;
  uint32_t memoryTypeIndex = ~0;
  for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
  {
    const auto& memoryType = memoryProperties.memoryTypes[i];
    if ((memoryTypeBits & 1) && (memoryType.propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal) == vk::MemoryPropertyFlagBits::eDeviceLocal)
    {
      memoryTypeIndex = i;
      break;
    }
    memoryTypeBits >>= 1;
  }
  assert(memoryTypeIndex != ~0);

  _depthMemory = vkr::DeviceMemory(
    _device,
    vk::MemoryAllocateInfo{
      .allocationSize = memoryRequirements.size,
      .memoryTypeIndex = memoryTypeIndex,
    });

  _depthImage.bindMemory(*_depthMemory, 0);

  _depthImageView = vkr::ImageView(
    _device,
    vk::ImageViewCreateInfo{
      .image = *_depthImage,
      .viewType = vk::ImageViewType::e2D,
      .format = _depthImageFormat,
      .subresourceRange = {
        .aspectMask = vk::ImageAspectFlagBits::eDepth,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1}});
}

void VulkanRenderer::uniformBuffer()
{
  _uniformBuffer = vkr::Buffer(
    _device,
    vk::BufferCreateInfo{
      .size = sizeof(glm::mat4x4),
      .usage = vk::BufferUsageFlagBits::eUniformBuffer,
      .sharingMode = vk::SharingMode::eExclusive});

  const auto memoryProperties = _physicalDevice.getMemoryProperties();
  const auto memoryRequirements = _uniformBuffer.getMemoryRequirements();
  uint32_t memoryTypeIndex = 0;

  // Find the right memory for the uniform buffer.
  for (; memoryTypeIndex < memoryProperties.memoryTypeCount; ++memoryTypeIndex)
  {
    const auto memoryTypeBit = 1 << memoryTypeIndex;
    if (memoryRequirements.memoryTypeBits & memoryTypeBit)
    {
      const auto memoryType = memoryProperties.memoryTypes[memoryTypeIndex];
      if (memoryType.propertyFlags & (vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent))
      {
        // we found the right memory, yippie!!
        break;
      }
    }
  }

  _uniformBufferMemory = vkr::DeviceMemory(
    _device,
    vk::MemoryAllocateInfo{
      .allocationSize = memoryRequirements.size,
      .memoryTypeIndex = memoryTypeIndex,
    });

  _uniformBuffer.bindMemory(*_uniformBufferMemory, 0);
}

void VulkanRenderer::renderPass()
{
  const std::array attachmentDescriptions = {
    // Surface attachment
    vk::AttachmentDescription{
      .format = _surfaceImageFormat,
      .samples = vk::SampleCountFlagBits::e1,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
      .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
      .initialLayout = vk::ImageLayout::eUndefined,
      .finalLayout = vk::ImageLayout::ePresentSrcKHR},
    // Depth attachment
    vk::AttachmentDescription{
      .format = _depthImageFormat,
      .samples = vk::SampleCountFlagBits::e1,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eDontCare,
      .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
      .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
      .initialLayout = vk::ImageLayout::eUndefined,
      .finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal},
  };

  const vk::AttachmentReference surfaceAttachmentReference = {
    .attachment = 0,
    .layout = vk::ImageLayout::eColorAttachmentOptimal};

  const vk::AttachmentReference depthAttachmentReference = {
    .attachment = 1,
    .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal};

  const vk::SubpassDescription subpassDescription = {
    .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
    .colorAttachmentCount = 1,
    .pColorAttachments = &surfaceAttachmentReference,
    .pDepthStencilAttachment = &depthAttachmentReference};

  _renderPass = vkr::RenderPass(
    _device,
    vk::RenderPassCreateInfo{
      .attachmentCount = 2,
      .pAttachments = attachmentDescriptions.data(),
      .subpassCount = 1,
      .pSubpasses = &subpassDescription});
}


std::array _materials = {
  arete::VulkanMaterial{}
};

std::array _meshes = {
  arete::VulkanMesh{}
};

void VulkanRenderer::shaders(arete::Engine& engine)
{
  arete::VulkanShader vertexShader;
  arete::VulkanShader fragmentShader;

  const auto& shaders = engine.shaders();
  for (const auto& [handle, shader]: shaders)
  {
    if (shader.stage() == arete::Shader::Stage::Fragment)
    {
      fragmentShader = {
        ._shader =  handle,
        ._vulkanShader = vkr::ShaderModule(
        _device,
        vk::ShaderModuleCreateInfo{
          .codeSize = shader.source().size(),
          .pCode = reinterpret_cast<const uint32_t*>(shader.source().data())
        })
      };
    }
    if (shader.stage() == arete::Shader::Stage::Vertex)
    {
      vertexShader = {
        ._shader =  handle,
        ._vulkanShader = vkr::ShaderModule(
        _device,
        vk::ShaderModuleCreateInfo{
          .codeSize = shader.source().size(),
          .pCode = reinterpret_cast<const uint32_t*>(shader.source().data())
        })
      };
    }
  }

  _materials[0] = {
    ._vertexShader =  std::move(vertexShader),
    ._fragmentShader = std::move(fragmentShader),
  };
}

void VulkanRenderer::pipeline()
{
  const vk::DescriptorSetLayoutBinding uniformDescriptorSetLayoutBinding{
    .descriptorType = vk::DescriptorType::eUniformBuffer,
    .descriptorCount = 1,
    .stageFlags = vk::ShaderStageFlagBits::eVertex};

  _uniformDescriptorLayout = vkr::DescriptorSetLayout(
    _device,
    vk::DescriptorSetLayoutCreateInfo{
      .bindingCount = 1,
      .pBindings = &uniformDescriptorSetLayoutBinding});

  // Push constants
  vk::PushConstantRange pushConstantRange {
    .stageFlags = vk::ShaderStageFlagBits::eVertex,
    .offset = 0,
    .size = sizeof(arete::PushConstants)
  };

  _pipelineLayout = vkr::PipelineLayout(
    _device,
    vk::PipelineLayoutCreateInfo{
      .setLayoutCount = 1,
      .pSetLayouts = &(*_uniformDescriptorLayout),
      .pushConstantRangeCount = 1,
      .pPushConstantRanges = &pushConstantRange,
    });

  // Uniform buffer
  const vk::DescriptorPoolSize uniformDescriptorPoolSize{
    .type = vk::DescriptorType::eUniformBuffer,
    .descriptorCount = 1};

  _uniformDescriptorPool = vkr::DescriptorPool(
    _device,
    vk::DescriptorPoolCreateInfo{
      .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
      .maxSets = 1,
      .poolSizeCount = 1,
      .pPoolSizes = &uniformDescriptorPoolSize});

  _uniformDescriptorSets = vkr::DescriptorSets(
    _device,
    vk::DescriptorSetAllocateInfo{
      .descriptorPool = *_uniformDescriptorPool,
      .descriptorSetCount = 1,
      .pSetLayouts = &(*_uniformDescriptorLayout)});

  const vk::DescriptorBufferInfo uniformDescriptorBufferInfo{
    .buffer = *_uniformBuffer,
    .offset = 0,
    .range = sizeof(glm::mat4x4)};

  const vk::WriteDescriptorSet writeUniformDescriptorSet{
    .dstSet = *_uniformDescriptorSets.front(),
    .descriptorCount = 1,
    .descriptorType = vk::DescriptorType::eUniformBuffer,
    .pBufferInfo = &uniformDescriptorBufferInfo};

  _device.updateDescriptorSets(writeUniformDescriptorSet, nullptr);

  std::array pipelineShaderStageCreateInfos{
    vk::PipelineShaderStageCreateInfo{
      .stage = vk::ShaderStageFlagBits::eVertex,
      .module = *(_materials[0]._vertexShader._vulkanShader),
      .pName = "main",
    },
    vk::PipelineShaderStageCreateInfo{
      .stage = vk::ShaderStageFlagBits::eFragment,
      .module = *(_materials[0]._fragmentShader._vulkanShader),
      .pName = "main",
    },
  };

  // Vertex buffer
  std::array vertexBindingDescriptions{
    vk::VertexInputBindingDescription{
      .binding = 0,
      .stride = sizeof(arete::Mesh::VertexElementType),
      .inputRate = vk::VertexInputRate::eVertex,
    }};

  std::array vertexAttributeDescriptions{
    vk::VertexInputAttributeDescription{
      .location = 0,
      .binding = 0,
      .format = vk::Format::eR32G32B32Sfloat,
      .offset = 0,
    }
  };

  vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{
    .vertexBindingDescriptionCount = vertexBindingDescriptions.size(),
    .pVertexBindingDescriptions = vertexBindingDescriptions.data(),
    .vertexAttributeDescriptionCount = vertexAttributeDescriptions.size(),
    .pVertexAttributeDescriptions = vertexAttributeDescriptions.data()};

  vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{
    .topology = vk::PrimitiveTopology::eTriangleList};

  vk::PipelineViewportStateCreateInfo viewportStateCreateInfo{
    .viewportCount = 1,
    .scissorCount = 1,
  };

  vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{
    .depthClampEnable = VK_FALSE,
    .rasterizerDiscardEnable = VK_FALSE,
    .polygonMode = vk::PolygonMode::eFill,
    .cullMode = vk::CullModeFlagBits::eBack,
    .frontFace = vk::FrontFace::eClockwise,
    .lineWidth = 1.0f,
  };

  vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo{
    .rasterizationSamples = vk::SampleCountFlagBits::e1};

  vk::StencilOpState stencilOpState{
    .failOp = vk::StencilOp::eKeep,
    .passOp = vk::StencilOp::eKeep,
    .depthFailOp = vk::StencilOp::eKeep,
    .compareOp = vk::CompareOp::eAlways,
  };

  vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{
    .depthTestEnable = true,
    .depthWriteEnable = true,
    .depthCompareOp = vk::CompareOp::eLessOrEqual,
    .depthBoundsTestEnable = false,
    .stencilTestEnable = false,
    .front = stencilOpState,
    .back = stencilOpState};

  vk::ColorComponentFlags colorComponentFlags(
    vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

  vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{
    .blendEnable = false,
    .srcColorBlendFactor = vk::BlendFactor::eZero,
    .dstColorBlendFactor = vk::BlendFactor::eZero,
    .colorBlendOp = vk::BlendOp::eAdd,
    .srcAlphaBlendFactor = vk::BlendFactor::eZero,
    .dstAlphaBlendFactor = vk::BlendFactor::eZero,
    .alphaBlendOp = vk::BlendOp::eAdd,
    .colorWriteMask = colorComponentFlags,
  };

  vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{
    .logicOpEnable = VK_FALSE,
    .logicOp = vk::LogicOp::eNoOp,
    .attachmentCount = 1,
    .pAttachments = &pipelineColorBlendAttachmentState,
    .blendConstants = {{1.0f, 1.0f, 1.0f, 1.0f}}};

  std::array dynamicStates = {
    vk::DynamicState::eViewport,
    vk::DynamicState::eScissor};

  vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{
    .dynamicStateCount = dynamicStates.size(),
    .pDynamicStates = dynamicStates.data()};

  _pipeline = vkr::Pipeline(
    _device,
    nullptr,
    vk::GraphicsPipelineCreateInfo{
      .stageCount = pipelineShaderStageCreateInfos.size(),
      .pStages = pipelineShaderStageCreateInfos.data(),
      .pVertexInputState = &vertexInputStateCreateInfo,
      .pInputAssemblyState = &inputAssemblyStateCreateInfo,
      .pTessellationState = nullptr,
      .pViewportState = &viewportStateCreateInfo,
      .pRasterizationState = &rasterizationStateCreateInfo,
      .pMultisampleState = &multisampleStateCreateInfo,
      .pDepthStencilState = &depthStencilStateCreateInfo,
      .pColorBlendState = &colorBlendStateCreateInfo,
      .pDynamicState = &pipelineDynamicStateCreateInfo,
      .layout = *_pipelineLayout,
      .renderPass = *_renderPass});
}

void VulkanRenderer::commands()
{
  _commandPool = vkr::CommandPool(
    _device,
    vk::CommandPoolCreateInfo{
      .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
      .queueFamilyIndex = _queueFamilyHints.graphicsFamily.value()});

  _commandBuffers = vkr::CommandBuffers(
    _device,
    vk::CommandBufferAllocateInfo{
      .commandPool = *_commandPool,
      .level = vk::CommandBufferLevel::ePrimary,
      .commandBufferCount = 2,
    });

  _graphicsQueue = vkr::Queue(
    _device, _queueFamilyHints.graphicsFamily.value(), 0);

  _presentQueue = vkr::Queue(
    _device, _queueFamilyHints.presentFamily.value(), 0);
}

void VulkanRenderer::setup()
{
  _extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  _extensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
  _layers.emplace_back("VK_LAYER_KHRONOS_validation");
  //_layers.emplace_back("VK_LAYER_RENDERDOC_Capture");

  const vk::ApplicationInfo applicationInfo{
    .pApplicationName = "Hello World",
    .applicationVersion = 1,
    .pEngineName = "VulkanEngine",
    .engineVersion = 1,
    .apiVersion = VK_API_VERSION_1_1,
  };

  // Extensions in contiguous array
  std::vector<const char*> extensions;
  extensions.reserve(_extensions.size());
  for (const auto& extension: _extensions)
  {
    extensions.emplace_back(extension.data());
  }

  // Layers in contiguous array
  std::vector<const char*> layers;
  extensions.reserve(_layers.size());
  for (const auto& layer: _layers)
  {
    layers.emplace_back(layer.data());
  }

  const vk::DebugUtilsMessengerCreateInfoEXT debugMessengerInfo = {
    .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose,
    .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
    .pfnUserCallback = [](
                         VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                         VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                         const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                         void* pUserData) -> VkBool32
    {
      printf("[Vulkan] %s\n", pCallbackData->pMessage);
      return VK_FALSE;
    }};

  const vk::InstanceCreateInfo instanceCreateInfo{
    .pNext = &debugMessengerInfo,
    .pApplicationInfo = &applicationInfo,
    .enabledLayerCount = static_cast<uint32_t>(layers.size()),
    .ppEnabledLayerNames = layers.data(),
    .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
    .ppEnabledExtensionNames = extensions.data(),
  };

  // Setup instance
  _instance = vkr::Instance(_ctx, instanceCreateInfo);

  // Setup debugger
  /*vkr::DebugUtilsMessengerEXT debugMessenger(
    _instance, debugMessengerInfo);*/
}


InFlightRendering::InFlightRendering(const VulkanRenderer& renderer, const VulkanEngine& engine)
    : _renderer(renderer), _engine(engine)
{
  const auto& device = _renderer._device;
  // Image available semaphores
  _imageAvailableSemaphores = {
    vkr::Semaphore(device, vk::SemaphoreCreateInfo{}),
    vkr::Semaphore(device, vk::SemaphoreCreateInfo{}),
  };

  // Image rendered semaphores
  _imageRenderedSemaphores = {
    vkr::Semaphore(device, vk::SemaphoreCreateInfo{}),
    vkr::Semaphore(device, vk::SemaphoreCreateInfo{}),
  };

  // In flight fences
  _inFlightFences = {
    vkr::Fence(
      device,
      vk::FenceCreateInfo {
        .flags = vk::FenceCreateFlagBits::eSignaled }),
    vkr::Fence(
      device,
      vk::FenceCreateInfo {
        .flags = vk::FenceCreateFlagBits::eSignaled }),
  };

  // Clear values.
  _clearValues = {
    vk::ClearValue {
      .color = vk::ClearColorValue {
        .float32 = {{ 0.2f, 0.2f, 0.2f, 0.2f}}
      }
    },
    vk::ClearValue {
      .depthStencil = vk::ClearDepthStencilValue {
        .depth = 1.0f,
        .stencil = 0,
      }
    }
  };
}

void InFlightRendering::draw()
{
  render();
  present();
  _inFlightFrameIndex = (_inFlightFrameIndex + 1) % MaxFramesInFlight;
}

void InFlightRendering::render()
{
  const auto& device = _renderer._device;
  const auto& frameFence
    = *_inFlightFences[_inFlightFrameIndex];

  const auto fenceWaitResult = device.waitForFences(
    frameFence, true, UINT64_MAX);
  assert(fenceWaitResult == vk::Result::eSuccess);
  device.resetFences(frameFence);

  const auto& imageAvailableSemaphore
    = *_imageAvailableSemaphores[_inFlightFrameIndex];
  const auto& imageRenderedSemaphore
    = *_imageRenderedSemaphores[_inFlightFrameIndex];

  const auto& swapchain = _renderer._swapChain;
  auto [result, imageIndex] = swapchain.acquireNextImage(
    UINT64_MAX, imageAvailableSemaphore);
  _currentImageIndex = imageIndex;

  assert(result == vk::Result::eSuccess);
  assert(imageIndex < _renderer._swapChainImageViews.size());

  const auto& commandBuffer = _renderer._commandBuffers[_inFlightFrameIndex];
  commandBuffer.reset();
  commandBuffer.begin(vk::CommandBufferBeginInfo{});

  const vk::RenderPassBeginInfo renderPassBeginInfo {
    .renderPass = *_renderer._renderPass,
    .framebuffer = *_renderer._framebuffers[imageIndex],
    .renderArea = vk::Rect2D {
      .offset = {0, 0},
      .extent = _renderer._surfaceCapabilities.currentExtent
    },
    .clearValueCount = static_cast<uint32_t>(_clearValues.size()),
    .pClearValues = _clearValues.data()
  };

  commandBuffer.beginRenderPass(
    renderPassBeginInfo, vk::SubpassContents::eInline
  );

  commandBuffer.bindPipeline(
    vk::PipelineBindPoint::eGraphics,
    *_renderer._pipeline
  );

  // Bind descriptor sets
  const auto& pipelineLayout = *_renderer._pipelineLayout;
  const auto& descriptorSet = *_renderer._uniformDescriptorSets.front();

  commandBuffer.pushConstants(
    pipelineLayout,
    vk::ShaderStageFlagBits::eVertex,
    0,
    vk::ArrayProxy<const arete::PushConstants>({_engine._pushConstants})
  );

  commandBuffer.bindDescriptorSets(
    vk::PipelineBindPoint::eGraphics,
    pipelineLayout,
    0,
    descriptorSet,
    nullptr
  );

  // Bind VBOs
  const auto& vertexBuffer = *_engine._mesh._vertexBuffer;
  commandBuffer.bindVertexBuffers(
    0, {vertexBuffer}, {0}
  );

  // Bind IBO
  const auto& indexBuffer = *_engine._mesh._indexBuffer;
  commandBuffer.bindIndexBuffer(
    indexBuffer, 0, vk::IndexType::eUint16
  );

  // Scissor
  commandBuffer.setScissor(
    0, vk::Rect2D(vk::Offset2D( 0, 0 ), _renderer._surfaceCapabilities.currentExtent));

  // Viewport
  commandBuffer.setViewport(
    0, vk::Viewport(
         0.0f,
         0.0f,
         static_cast<float>(_renderer._surfaceCapabilities.currentExtent.width),
         static_cast<float>(_renderer._surfaceCapabilities.currentExtent.height),
         0.0f,
         1.0f
         )
  );

  commandBuffer.drawIndexed(
    36, 1, 0, 0, 0
  );

  commandBuffer.endRenderPass();
  commandBuffer.end();

  const vk::PipelineStageFlags waitDestinationStageMask(
    vk::PipelineStageFlagBits::eColorAttachmentOutput);

  const vk::SubmitInfo submitInfo {
    .waitSemaphoreCount = 1,
    .pWaitSemaphores = &imageAvailableSemaphore,
    .pWaitDstStageMask = &waitDestinationStageMask,
    .commandBufferCount = 1,
    .pCommandBuffers = &(*commandBuffer),
    .signalSemaphoreCount = 1,
    .pSignalSemaphores = &imageRenderedSemaphore};

  const auto& graphicsQueue
    = _renderer._graphicsQueue;
  graphicsQueue.submit(submitInfo, frameFence);
}

void InFlightRendering::present()
{
  const auto& swapchain = _renderer._swapChain;
  const auto& frameRenderedSemaphore
    = *_imageRenderedSemaphores[_inFlightFrameIndex];

  vk::PresentInfoKHR presentInfoKHR {
    .waitSemaphoreCount = 1,
    .pWaitSemaphores = &frameRenderedSemaphore,
    .swapchainCount = 1,
    .pSwapchains = &(*swapchain),
    .pImageIndices = &_currentImageIndex};

  const auto& presentQueue = _renderer._presentQueue;
  assert(presentQueue.presentKHR(presentInfoKHR) == vk::Result::eSuccess);
}

} // namespace vulkan

