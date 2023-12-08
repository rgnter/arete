#include "arete/vulkan.hpp"
#include "arete/input/glfwInput.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>

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
  struct Camera {
    // Selfie / Position camera at the front of cube
    glm::vec3 pos { 0, 0, 6.0f };
    glm::quat rot { 1.0f, 0, 0, 0 };
  } cam;

  _pushConstants.proj = glm::perspective(
    glm::radians<float>(45.0f),
    static_cast<float>(_display.width) / static_cast<float>(_display.height),
    0.1f,
    100.0f
  );

  _pushConstants.model = glm::mat4x4( 1.0f );

  // Changing camera orientation example
  cam.rot = cam.rot * glm::angleAxis(glm::pi<float>(), glm::vec3(0, 1, 0));

  // Selfie / Look from front of the cube at 0,0,0
  _pushConstants.view = glm::lookAt(
    cam.pos,
    cam.pos + glm::vec3(0, 0, 1) * cam.rot,
    glm::vec3(0, 1, 0) * cam.rot
  );

  _pushConstants.clip = glm::mat4x4(
    1.0f,  0.0f, 0.0f, 0.0f,
    0.0f, -1.0f, 0.0f, 0.0f,
    0.0f,  0.0f, 0.5f, 0.0f,
    0.0f,  0.0f, 0.5f, 1.0f);  // vulkan clip space has inverted y and half z !

  _display.setup(_renderer);

  _glfwInput.setup(_display._window);

  // auto & actionMap = _glfwInput.createActionMap();

  // auto & action2 = actionMap.createAction(DataType::1D, );
  // auto & action2 = actionMap.createAction(DataType::2D);

  // action.AddKey(Key, )

  const std::string moveActionName = "Move";
  arete::input::InputAction<glm::vec3> moveAction(moveActionName);
  moveAction.mapInput(
    {
      arete::input::InputMapping<glm::vec3> {
        .inputKey = arete::input::InputKey::KEY_W,
        .axis = glm::vec3(0, 0, 1)
      },
      arete::input::InputMapping<glm::vec3> {
        .inputKey = arete::input::InputKey::KEY_A,
        .axis = glm::vec3(-1, 0, 0)
      },
      arete::input::InputMapping<glm::vec3> {
        .inputKey = arete::input::InputKey::KEY_S,
        .axis = glm::vec3(0, 0, -1)
      },
      arete::input::InputMapping<glm::vec3> {
        .inputKey = arete::input::InputKey::KEY_D,
        .axis = glm::vec3(1, 0, 0)
      },
      arete::input::InputMapping<glm::vec3> {
        .inputKey = arete::input::InputKey::KEY_E,
        .axis = glm::vec3(0, 1, 0)
      },
      arete::input::InputMapping<glm::vec3> {
        .inputKey = arete::input::InputKey::KEY_Q,
        .axis = glm::vec3(0, -1, 0)
      }
    }
  );

  // link behaviour to action callback
  glm::vec3 cameraInput(0);
  moveAction.performed = [&](const auto & event, const auto value) {
    cameraInput = value;
  };

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

  const auto& mesh = getMesh(_mesh._mesh);
  _mesh.indexBuffer(
    _renderer._device,
    _renderer._physicalDevice,
    mesh
  );
  _mesh.vertexBuffer(
    _renderer._device,
    _renderer._physicalDevice,
    mesh
  );

  InFlightRendering rendering(_renderer, *this);

  float rotationY = 0;
  float rotationX = 0;
  auto uniform = reinterpret_cast<glm::mat4x4*>(
    _renderer._uniformBufferMemory.mapMemory(0, sizeof(glm::mat4x4)));

  
  arete::TickClock tickClock;
  arete::TickClock physicsTickClock;
  float deltaTime;
  float physicsDeltaTime;

  while(!glfwWindowShouldClose(_display._window))
  {
    _glfwInput.processInput();

    // rotationY = 0;
    // rotationX = 0;

    // if(glfwGetKey(_display._window, GLFW_KEY_RIGHT) == GLFW_TRUE)
    // {
    //   rotationY = -glm::radians<float>(1);
    // }
    // else if(glfwGetKey(_display._window, GLFW_KEY_LEFT) == GLFW_TRUE)
    // {
    //   rotationY = glm::radians<float>(1);
    // }

    // if(glfwGetKey(_display._window, GLFW_KEY_UP) == GLFW_TRUE)
    // {
    //   rotationX = -glm::radians<float>(1);
    // }
    // else if(glfwGetKey(_display._window, GLFW_KEY_DOWN) == GLFW_TRUE)
    // {
    //   rotationX = glm::radians<float>(1);
    // }

    // if (rotationY)
    //   _pushConstants.model = glm::rotate(_pushConstants.model, rotationY, {0,1,0});
    // if (rotationX)
    //   _pushConstants.model = glm::rotate(_pushConstants.model, rotationX, {1,0,0});

    cam.pos += (cameraInput * cam.rot) * 0.01f; // deltaTime
    _pushConstants.view = glm::lookAt(
      cam.pos,
      cam.pos + glm::vec3(0, 0, 1) * cam.rot,
      glm::vec3(0, 1, 0) * cam.rot
    );


    // tick
    if (tickClock.tick(deltaTime))
    {
      // update
      std::cout << deltaTime << "\n";
    }
    
    // physicsTick
    if (physicsTickClock.tick(physicsDeltaTime))
    {
      // update
      // std::cout << physicsDeltaTime << "\n";
    }

    rendering.draw();

    if(glfwGetKey(_display._window, GLFW_KEY_ESCAPE))
    {
      glfwSetWindowShouldClose(_display._window, GLFW_TRUE);
    }
  }
}

}