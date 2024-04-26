#include "arete/vulkan.hpp"

namespace vulkan
{

void Display::setup(VulkanRenderer& renderer)
{
  if (!glfwInit())
    throw std::runtime_error("Couldn't initialize GLFW.");

  if(!glfwVulkanSupported())
    throw std::runtime_error("Vulkan is not supported");

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  _window = glfwCreateWindow(
    width, height, "Arete", nullptr, nullptr);

  // Copy GLFW extensions
  uint32_t glfwExtensionCount = 0;
  auto glfwExtensions = glfwGetRequiredInstanceExtensions(
    &glfwExtensionCount);
  for (int i = 0; i < glfwExtensionCount; ++i)
  {
    renderer._extensions.emplace_back(glfwExtensions[i]);
  }
}

} // namespace vulkan