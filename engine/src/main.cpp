#include <cstdio>

#include <arete/vulkan.hpp>

int main(int argc, char** argv)
{
  const auto readSpvBinary = [](const std::filesystem::path& shaderBinaryPath) {
    const auto size = std::filesystem::file_size(shaderBinaryPath);
    std::ifstream input(shaderBinaryPath, std::ios::binary);
    if (input.bad())
      throw std::runtime_error(
        std::format("Couldn't find shader at '{}'", shaderBinaryPath.c_str()));

    std::vector<uint8_t> buffer(size);
    input.read(reinterpret_cast<char*>(buffer.data()), size);
    return buffer;
  };

  vulkan::VulkanEngine engine;

  engine.createShader(
    arete::Shader::Stage::Vertex,
    readSpvBinary("resources/cube-vert.spv"));
  engine.createShader(
    arete::Shader::Stage::Fragment,
    readSpvBinary("resources/cube-frag.spv"));

  engine.run();
}