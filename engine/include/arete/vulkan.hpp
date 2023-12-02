#ifndef ARETE_VULKAN_HPP
#define ARETE_VULKAN_HPP

#include "arete/engine.hpp"

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string_view>
#include <filesystem>
#include <array>
#include <memory>
#include <fstream>
#include <format>
#include <vector>

namespace arete
{

namespace vkr = vk::raii;

//! Finds memory type satisfying memory requirements.
//! @param memoryProperties Memory properties.
//! @param memoryRequirements Memory requirements.
//! @returns Memory type index.
//! @throws If no appropriate memory was found.
uint32_t vulkanFindMemoryType(const vk::PhysicalDeviceMemoryProperties& memoryProperties,
                                 const vk::MemoryRequirements& memoryRequirements);

//! Vulkan material.
struct VulkanMaterial
{
  vkr::ShaderModule _fragmentShader { nullptr };
  vkr::ShaderModule _vertexShader { nullptr };
};

//! Vulkan mesh.
struct VulkanMesh
{
  vkr::Buffer _vertexBuffer { nullptr };
  vkr::DeviceMemory _vertexBufferMemory { nullptr };

  vkr::Buffer _indexBuffer { nullptr };
  vkr::DeviceMemory _indexBufferMemory { nullptr };

  void indexBuffer(const vkr::Device& device, const vkr::PhysicalDevice& physicalDevice, const Mesh& mesh);

  void vertexBuffer(const vkr::Device& device, const vkr::PhysicalDevice& physicalDevice, const Mesh& mesh);
};

} // namespace arete

namespace vulkan
{

namespace vkr = vk::raii;

//! VulkanRenderer.
class VulkanRenderer
{
public:
  //! Setup surface.
  void surface(GLFWwindow* window);

  //! Setup physical device.
  void physicalDevice();

  //! Setup device and queues.
  void logicalDevice();

  //! Setup swap chain.
  void swapChain();

  //! Setup frame buffers.
  void framebuffers();

  //! Setup depth buffer.
  void depthBuffer();

  //! Setup uniform buffer.
  void uniformBuffer();

  //! Setup pipeline.
  void pipeline();

  //! Setup render pass.
  void renderPass();

  //! Setup shaders.
  void shaders(arete::Engine& engine);

  //! Setup command pool and command buffers.
  void commands();

  //! Setup
  void setup();

public:
  const vkr::Context _ctx {};
  vkr::Instance _instance { nullptr };
  vkr::PhysicalDevice _physicalDevice { nullptr };
  vkr::Device _device { nullptr };

  vkr::Queue _graphicsQueue { nullptr };
  vkr::Queue _presentQueue { nullptr };

  vkr::CommandPool _commandPool { nullptr };
  vkr::CommandBuffers _commandBuffers { nullptr };

  vkr::Image _depthImage { nullptr };
  vk::Format _depthImageFormat {};
  vkr::ImageView _depthImageView { nullptr };
  vkr::DeviceMemory _depthMemory { nullptr };

  vkr::DeviceMemory _uniformBufferMemory { nullptr };
  vkr::Buffer _uniformBuffer { nullptr };

  vkr::DescriptorSetLayout _uniformDescriptorLayout { nullptr };
  vkr::DescriptorPool _uniformDescriptorPool { nullptr };
  vkr::DescriptorSets _uniformDescriptorSets { nullptr };

  vkr::SurfaceKHR _surface { nullptr };
  vk::SurfaceCapabilitiesKHR _surfaceCapabilities {};
  vkr::SwapchainKHR _swapChain { nullptr };
  std::vector<vkr::ImageView> _swapChainImageViews;
  vk::Format _surfaceImageFormat {};
  std::vector<vkr::Framebuffer> _framebuffers;

  vkr::RenderPass _renderPass { nullptr };
  vkr::Pipeline _pipeline { nullptr };
  vkr::PipelineLayout _pipelineLayout { nullptr };
private:
  struct QueueFamilyHints
  {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
  } _queueFamilyHints;

public:
  std::vector<std::string_view> _extensions;
  std::vector<std::string_view> _layers;

  std::vector<std::string_view> _devExtensions;
  std::vector<std::string_view> _devLayers;
};


// Display.
class Display
{
public:
  //! Sets up the display.
  void setup(VulkanRenderer& renderer);

public:
  GLFWwindow* _window;
  int width = 1920;
  int height = 1080;
};

static constexpr int32_t MaxFramesInFlight = 2;

class InFlightRendering
{
public:
  explicit InFlightRendering(const VulkanRenderer& renderer);
  /**
   * Draws frame.
   */
  void draw();

private:
  /**
   * Renders image in swapchain.
   */
  void render();

  /**
   * Presents rendered image to surface.
   */
  void present();

private:
private:
  const VulkanRenderer& _renderer;

  std::array<vkr::Semaphore, MaxFramesInFlight> _imageAvailableSemaphores
    {
      vkr::Semaphore { nullptr }, vkr::Semaphore { nullptr }
    };

  std::array<vkr::Semaphore, MaxFramesInFlight> _imageRenderedSemaphores
    {
      vkr::Semaphore { nullptr }, vkr::Semaphore { nullptr }
    };

  std::array<vkr::Fence, MaxFramesInFlight> _inFlightFences
    {
      vkr::Fence { nullptr }, vkr::Fence { nullptr }
    };

  std::array<vk::ClearValue, MaxFramesInFlight> _clearValues {};

private:
  uint32_t _inFlightFrameIndex = 0;
  uint32_t _currentImageIndex = 0;
};

class VulkanEngine :
    public arete::Engine
{

public:
  arete::ShaderHandle createShader(
    arete::Shader::Stage stage,
    const std::vector<uint8_t>& source) override
  {
    return Engine::createShader(stage, source);
  }

  arete::MaterialHandle createMaterial(
    arete::ShaderHandle vertexShader,
    arete::ShaderHandle fragmentShader) override
  {
    return Engine::createMaterial(vertexShader, fragmentShader);
  }

  arete::MeshHandle createMesh(
    arete::MaterialHandle material,
    const arete::Mesh::Vertices& vertices,
    const arete::Mesh::Indices& indices) override
  {
    return Engine::createMesh(material, vertices, indices);
  }

  void run() override;

private:
  VulkanRenderer _renderer;
  Display _display;
};

} // namespace vulkan

#endif//ARETE_VULKAN_HPP
