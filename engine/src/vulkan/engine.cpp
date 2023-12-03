#include "arete/vulkan.hpp"

namespace arete
{

uint32_t vulkanFindMemoryType(const vk::PhysicalDeviceMemoryProperties& memoryProperties, const vk::MemoryRequirements& memoryRequirements)
{
  int memoryTypeIndex = 0;

  for (; memoryTypeIndex < memoryProperties.memoryTypeCount; ++memoryTypeIndex)
  {
    const auto memoryTypeBit = 1 << memoryTypeIndex;
    if (memoryRequirements.memoryTypeBits & memoryTypeBit)
    {
      const auto memoryType = memoryProperties.memoryTypes[memoryTypeIndex];
      if (memoryType.propertyFlags & (vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent))
      {
        // we found the right memory, yippie!!
        return memoryTypeIndex;
      }
    }
  }

  throw std::runtime_error("Couldn't find the right memory type");
}

void VulkanMesh::indexBuffer(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, const arete::Mesh& mesh)
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
  uint32_t memoryTypeIndex = vulkanFindMemoryType(memoryProperties, memoryRequirements);

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

void VulkanMesh::vertexBuffer(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, const arete::Mesh& mesh)
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
  uint32_t memoryTypeIndex = vulkanFindMemoryType(memoryProperties, memoryRequirements);

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

  auto proj = glm::perspective(
    glm::radians<float>(45.0f), static_cast<float>(_display.width) / static_cast<float>(_display.height), 0.1f, 100.0f);
  auto model = glm::mat4x4( 1.0f );

  // Changing camera orientation example
  cam.rot = cam.rot * glm::angleAxis(glm::pi<float>(), glm::vec3(0, 1, 0));

  // Selfie / Look from front of the cube at 0,0,0
  const auto view = glm::lookAt(
    cam.pos,
    cam.pos + glm::vec3(0, 0, 1) * cam.rot,
    glm::vec3(0, 1, 0) * cam.rot
  );

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

}