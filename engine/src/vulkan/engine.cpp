#include "arete/vulkan.hpp"
#include "arete/input/glfwInput.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <thread>
#include <iostream>
#include <chrono>

namespace arete
{

//! Finds memory type satisfying memory requirements.
//! @param memoryProperties Memory properties.
//! @param memoryRequirements Memory requirements.
//! @param memoryFlags Memory flags.
//! @returns Memory type index.
//! @throws If no appropriate memory was found.
uint32_t vulkanFindMemoryType(
  const vk::PhysicalDeviceMemoryProperties& memoryProperties,
  const vk::MemoryRequirements& memoryRequirements,
  const vk::MemoryPropertyFlags memoryFlags)
{
  int memoryTypeIndex = 0;

  for (; memoryTypeIndex < memoryProperties.memoryTypeCount; ++memoryTypeIndex)
  {
    const auto memoryTypeBit = 1 << memoryTypeIndex;
    if (memoryRequirements.memoryTypeBits & memoryTypeBit)
    {
      const auto memoryType = memoryProperties.memoryTypes[memoryTypeIndex];
      if (memoryType.propertyFlags & memoryFlags)
      {
        // we found the right memory, yippie!!
        return memoryTypeIndex;
      }
    }
  }

  throw std::runtime_error("Couldn't find the right memory type");
}

void VulkanMesh::indexBuffer(
  const vkr::Device& device,
  const vkr::PhysicalDevice& physicalDevice,
  const Mesh& mesh)
{
  const auto& indices = mesh.indices();
  const auto indicesSize = indices.size() * sizeof(Mesh::IndexElementType);

  _indexBuffer = vkr::Buffer(
    device,
    vk::BufferCreateInfo {
      .size = indicesSize,
      .usage = vk::BufferUsageFlagBits::eIndexBuffer,
      .sharingMode = vk::SharingMode::eExclusive,
    }
  );

  const auto memoryProperties = physicalDevice.getMemoryProperties();
  const auto memoryRequirements = _indexBuffer.getMemoryRequirements();
  const uint32_t memoryTypeIndex = vulkanFindMemoryType(
    memoryProperties,
    memoryRequirements,
    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    );

  _indexBufferMemory = vkr::DeviceMemory(
    device,
    vk::MemoryAllocateInfo {
      .allocationSize = memoryRequirements.size,
      .memoryTypeIndex = memoryTypeIndex
    });

  auto memory = static_cast<uint8_t*>(
    _indexBufferMemory.mapMemory(0, memoryRequirements.size));
  std::memcpy(memory,indices.data(), indicesSize);
  _indexBufferMemory.unmapMemory();
  _indexBuffer.bindMemory(*_indexBufferMemory, 0);
}

void VulkanMesh::vertexBuffer(
  const vkr::Device& device,
  const vkr::PhysicalDevice& physicalDevice,
  const Mesh& mesh)
{
  const auto& vertices = mesh.vertices();
  const auto verticesSize = vertices.size() * sizeof(Mesh::VertexElementType);

  _vertexBuffer = vkr::Buffer(
    device,
    vk::BufferCreateInfo {
      .size = verticesSize,
      .usage = vk::BufferUsageFlagBits::eVertexBuffer,
      .sharingMode = vk::SharingMode::eExclusive
    });

  const auto memoryProperties = physicalDevice.getMemoryProperties();
  const auto memoryRequirements = _vertexBuffer.getMemoryRequirements();
  const uint32_t memoryTypeIndex = vulkanFindMemoryType(
    memoryProperties,
    memoryRequirements,
    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  _vertexBufferMemory = vkr::DeviceMemory(
    device,
    vk::MemoryAllocateInfo {
      .allocationSize = memoryRequirements.size,
      .memoryTypeIndex = memoryTypeIndex
    });

  auto memory = static_cast<uint8_t*>(
    _vertexBufferMemory.mapMemory(0, memoryRequirements.size));
  std::memcpy(memory, vertices.data(), verticesSize);
  _vertexBufferMemory.unmapMemory();
  _vertexBuffer.bindMemory(*_vertexBufferMemory, 0);
}

} // namespace arete

namespace vulkan
{

void VulkanEngine::run()
{
  // Initialize camera
  struct Camera {
    // Selfie / Position camera at the front of cube
    glm::vec3 pos { 0, 0, 6.0f };
    glm::quat rot { 1.0f, 0, 0, 0 };
  } cam;
  cam.rot = cam.rot * glm::angleAxis(glm::pi<float>(), glm::vec3(0, 1, 0));

  // Initialize push constants
  {
    _shaderMatrices.proj = glm::perspective(
      glm::radians<float>(45.0f),
      static_cast<float>(_display.width) / static_cast<float>(_display.height),
      0.1f,
      100.0f
    );

    _shaderMatrices.model = glm::mat4x4( 1.0f );

    // Selfie / Look from front of the cube at 0,0,0
    _shaderMatrices.view = glm::lookAt(
      cam.pos,
      cam.pos + glm::vec3(0, 0, 1) * cam.rot,
      glm::vec3(0, 1, 0) * cam.rot
    );

    _shaderMatrices.clip = glm::mat4x4(
      1.0f,  0.0f, 0.0f, 0.0f,
      0.0f, -1.0f, 0.0f, 0.0f,
      0.0f,  0.0f, 0.5f, 0.0f,
      0.0f,  0.0f, 0.5f, 1.0f);  // vulkan clip space has inverted y and half z !

    _pushConstants.time = 0;
    _pushConstants.model = _shaderMatrices.model; 
    _pushConstants.mvp = _shaderMatrices.clip * _shaderMatrices.proj * _shaderMatrices.view * _shaderMatrices.model; 
    // _pushConstantsCore.cameraPos = cam.pos;
    // _pushConstantsCore.cameraDir = glm::vec3(0, 0, 1) * cam.rot;
  }

  // Initialize renderer
  {
    _display.setup(_renderer);

    _renderer.setup();
    _renderer.surface(_display._window);
    _renderer.physicalDevice();
    _renderer.logicalDevice();

    _renderer.shaders(*this);

    _renderer.swapChain();

    _renderer.depthBuffer();
    _renderer.uniformBuffer();

    _renderer.renderPass();

    _renderer.framebuffers();

    _renderer.pipeline();

    _renderer.commands();
  }

  // Initialize input
  arete::input::ActionMap cameraActionMap;
  glm::vec3 cameraMoveInput(0);
  glm::vec3 cameraLookInput(0);
  bool cameraDragInput = false;
  const float sensitivity = .002f;
  const float speed = 2.f;
  {
    _glfwInput.bind(_display._window);

    auto angleBetween = [](
      glm::vec3 a,
      glm::vec3 b,
      glm::vec3 origin
    ) {
      glm::vec3 da = glm::normalize(a-origin);
      glm::vec3 db = glm::normalize(b-origin);
      return glm::acos(glm::dot(da, db));
    };

    glm::vec3 camForward = glm::vec3(0, 0, 1) * cam.rot;
    glm::vec3 forwardYPlane = glm::vec3(camForward.x, 0, camForward.z);
    
    float yAngle = angleBetween(
      glm::vec3(0, 0, 1),
      forwardYPlane,
      glm::vec3(0)
    );

    float xAngle = angleBetween(
      forwardYPlane,
      camForward,
      glm::vec3(0)
    );

    cameraLookInput.x = yAngle;
    cameraLookInput.y = xAngle;

    auto * move = cameraActionMap.createAction<glm::vec3>("move", arete::input::Composite::VALUE);
    auto * look = cameraActionMap.createAction<glm::vec3>("look", arete::input::Composite::DELTA);
    auto * drag = cameraActionMap.createAction<bool>("drag");
    
    move->mapInput(std::vector<arete::input::InputMapping<glm::vec3>> {
      {
        .inputKey = arete::input::InputKey::KEYBOARD_W,
        .axis = glm::vec3(0, 0, 1)
      },
      {
        .inputKey = arete::input::InputKey::KEYBOARD_A,
        .axis = glm::vec3(1, 0, 0)
      },
      {
        .inputKey = arete::input::InputKey::KEYBOARD_S,
        .axis = glm::vec3(0, 0, -1)
      },
      {
        .inputKey = arete::input::InputKey::KEYBOARD_D,
        .axis = glm::vec3(-1, 0, 0)
      },
      {
        .inputKey = arete::input::InputKey::KEYBOARD_E,
        .axis = glm::vec3(0, 1, 0)
      },
      {
        .inputKey = arete::input::InputKey::KEYBOARD_Q,
        .axis = glm::vec3(0, -1, 0)
      }
    });

    move->performed = [&](const auto & event, const auto value) {
      cameraMoveInput = value;
    };

    look->mapInput(std::vector<arete::input::InputMapping<glm::vec3>> {
      {
        .inputKey = arete::input::InputKey::MOUSE_POS_X,
        .axis = glm::vec3(1, 0, 0)
      },
      {
        .inputKey = arete::input::InputKey::MOUSE_POS_Y,
        .axis = glm::vec3(0, 1, 0)
      }
    });

    look->performed = [&](const auto & event, const auto value) {
      if (cameraDragInput)
      {
        cameraLookInput.x += value.x * sensitivity;

        // reset horizontal rotation around Y axis if rotation was more then 360
        // to prevent float overflow, that -.5f is for floor rounding
        float pi = glm::pi<float>();
        int test = static_cast<int>((cameraLookInput.x / pi * 360) - .5f) / 360;
        if (glm::abs(test) > 0)
        {
          cameraLookInput.x -= test * 2 * pi;
        }
        
        // clamp vertical rotation around local X axis
        cameraLookInput.y = glm::clamp(cameraLookInput.y - value.y * sensitivity, -pi / 2.0001f, pi / 2.0001f);
      }
    };

    drag->mapInput(std::vector<arete::input::InputMapping<bool>> {
      {
        .inputKey = arete::input::InputKey::MOUSE_RIGHT,
      }
    });

    drag->started = [&](const auto & event, const auto value) {
      _glfwInput.setMouseMode(arete::input::MouseMode::DISABLED);
    };
    drag->performed = [&](const auto & event, const auto value) {
      cameraDragInput = value;
    };
    drag->canceled = [&](const auto & event, const auto value) {
      _glfwInput.setMouseMode(arete::input::MouseMode::NORMAL);
    };
  
    cameraActionMap.bind(_glfwInput);
  }

  // Context and In Flight Rendering
  const auto& mesh = getMesh(_renderer._mesh._mesh);
  _renderer._mesh.indexBuffer(
    _renderer._device,
    _renderer._physicalDevice,
    mesh
  );
  _renderer._mesh.vertexBuffer(
    _renderer._device,
    _renderer._physicalDevice,
    mesh
  );

  InFlightRendering rendering(_renderer, *this);

  // Engine ticking
  arete::TickClock tickClock(0);
  arete::TickClock physicsTickClock(60);

//  arete::Composer composer;
//
//  _physicsSystems.tick();
//  _drawSystems.tick(composer);

  while(!glfwWindowShouldClose(_display._window))
  {
    _glfwInput.processInput();

    const auto engineTick = tickClock.tick();
    // tick
    if (engineTick.shouldTick)
    {
      // update
      _pushConstants.time += engineTick.deltaTime;

      cam.pos += (cameraMoveInput * cam.rot) * engineTick.deltaTime * speed;
      if (cameraDragInput)
      {
        cam.rot = 
          glm::angleAxis(cameraLookInput.y, glm::vec3(1, 0, 0))
          *
          glm::angleAxis(cameraLookInput.x, glm::vec3(0, 1, 0))
        ;
      }
      
      // _pushConstantsCore.cameraPos = cam.pos;
      // _pushConstantsCore.cameraDir = glm::vec3(0, 0, 1) * cam.rot;

      _shaderMatrices.view = glm::lookAt(
        cam.pos,
        cam.pos + glm::vec3(0, 0, 1) * cam.rot,
        glm::vec3(0, 1, 0) * cam.rot
      );

      _pushConstants.mvp = _shaderMatrices.clip * _shaderMatrices.proj * _shaderMatrices.view * _shaderMatrices.model;
    }

    rendering.draw();

    if(glfwGetKey(_display._window, GLFW_KEY_ESCAPE))
    {
      glfwSetWindowShouldClose(_display._window, GLFW_TRUE);
    }
  }
}

} // namespace vulkan