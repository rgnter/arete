#ifndef ARETE_VULKAN_HPP
#define ARETE_VULKAN_HPP

#include "arete/engine.hpp"

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <string_view>
#include <filesystem>
#include <array>
#include <memory>
#include <fstream>
#include <format>
#include <vector>

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

  //! Setup vertex buffer.
  void vertexBuffer();

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

  vkr::DeviceMemory _vertexBufferMemory { nullptr };
  vkr::Buffer _vertexBuffer { nullptr };

  vkr::SurfaceKHR _surface { nullptr };
  vk::SurfaceCapabilitiesKHR _surfaceCapabilities {};
  vkr::SwapchainKHR _swapChain { nullptr };
  std::vector<vkr::ImageView> _swapChainImageViews;
  vk::Format _surfaceImageFormat {};
  std::vector<vkr::Framebuffer> _framebuffers;

  vkr::RenderPass _renderPass { nullptr };
  vkr::Pipeline _pipeline { nullptr };
  vkr::PipelineLayout _pipelineLayout { nullptr };

  vkr::ShaderModule _fragmentShader { nullptr };
  vkr::ShaderModule _vertexShader { nullptr };

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
  void setup(VulkanRenderer& renderer) {
    if (!glfwInit())
      throw std::runtime_error("Couldn't initialize GLFW.");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    _window = glfwCreateWindow(
      width, height, "Title", nullptr, nullptr);

    // Copy GLFW extensions
    uint32_t glfwExtensionCount = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(
      &glfwExtensionCount);
    for (int i = 0; i < glfwExtensionCount; ++i)
    {
      renderer._extensions.emplace_back(glfwExtensions[i]);
    }
  }

public:
  GLFWwindow* _window;
  int width = 800;
  int height = 600;
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

class VulkanEngine : public arete::Engine
{

public:
  void run() override
  {
    auto position = glm::vec3(-6, 3, -5);
    auto proj = glm::perspective(
      glm::radians<float>(45.0f), 1.0f, 0.1f, 100.0f);
    auto model = glm::mat4x4( 1.0f );

    const auto view = glm::lookAt(
      position,
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, -1.0f, 0.0f));

    const auto clip = glm::mat4x4(
      1.0f,  0.0f, 0.0f, 0.0f,
      0.0f, -1.0f, 0.0f, 0.0f,
      0.0f,  0.0f, 0.5f, 0.0f,
      0.0f,  0.0f, 0.5f, 1.0f);  // vulkan clip space has inverted y and half z !

    _display.setup(_renderer);
    _renderer.setup();
    _renderer.surface(_display._window);
    _renderer.physicalDevice();
    _renderer.logicalDevice();

    _renderer.shaders(*this);

    _renderer.swapChain();

    _renderer.depthBuffer();
    _renderer.uniformBuffer();
    _renderer.vertexBuffer();

    _renderer.renderPass();

    _renderer.framebuffers();

    _renderer.pipeline();

    _renderer.commands();

    InFlightRendering rendering(_renderer);

    float rotationY = 0;
    float rotationX = 0;
    auto uniform = reinterpret_cast<glm::mat4x4*>(
      _renderer._uniformBufferMemory.mapMemory(0, sizeof(glm::mat4x4)));

    while(!glfwWindowShouldClose(_display._window))
    {
      glfwPollEvents();

      rotationY = 0;
      rotationX = 0;

      if(glfwGetKey(_display._window, GLFW_KEY_RIGHT) == GLFW_TRUE)
      {
        rotationY = -glm::radians<float>(1);
      }
      else if(glfwGetKey(_display._window, GLFW_KEY_LEFT) == GLFW_TRUE)
      {
        rotationY = glm::radians<float>(1);
      }

      if(glfwGetKey(_display._window, GLFW_KEY_UP) == GLFW_TRUE)
      {
        rotationX = -glm::radians<float>(1);
      }
      else if(glfwGetKey(_display._window, GLFW_KEY_DOWN) == GLFW_TRUE)
      {
        rotationX = glm::radians<float>(1);
      }

      if (rotationY)
        model = glm::rotate(model, rotationY, {0,1,0});
      if (rotationX)
        model = glm::rotate(model, rotationX, {1,0,0});

      *uniform = clip * proj * view * model;

      rendering.draw();

      if(glfwGetKey(_display._window, GLFW_KEY_ESCAPE))
      {
        glfwSetWindowShouldClose(_display._window, GLFW_TRUE);
      }

    }
  }

private:
  VulkanRenderer _renderer;
  Display _display;
};

} // namespace vulkan

#endif//ARETE_VULKAN_HPP
