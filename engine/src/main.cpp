#include <cstdio>

#include <arete/vulkan.hpp>
#include <fstream>
#include <filesystem>

int main(int argc, char** argv)
{
  const auto readMeshBinary = [](const std::filesystem::path& shaderBinaryPath)
  {
    std::ifstream input(shaderBinaryPath, std::ios::binary);
    if (!input.is_open())
      throw std::runtime_error(
        std::format("Couldn't find mesh at '{}'", shaderBinaryPath.c_str()));

    std::vector<glm::f32vec3> vertices;
    std::vector<glm::u16vec3> indices;

    while(!input.eof())
    {
      std::string keyword;
      input >> keyword;
      if (keyword == "v")
      {
        float x, y, z;
        input >> x >> y >> z;
        vertices.emplace_back(x, y, z);
      }
      else if (keyword == "f")
      {
        uint16_t i, j, k;
        input >> i >> j >> k;
        indices.emplace_back(i, j, k);
      }
    }

    return std::pair{vertices, indices};
  };

  const auto readSpvBinary = [](const std::filesystem::path& shaderBinaryPath) -> std::vector<uint8_t>
  {
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

  auto vertexShader = engine.createShader(
    arete::Shader::Stage::Vertex,
    readSpvBinary("resources/cube-vert.spv"));
  auto fragmentShader = engine.createShader(
    arete::Shader::Stage::Fragment,
    readSpvBinary("resources/cube-frag.spv"));

  auto material = engine.createMaterial(
    vertexShader,
    fragmentShader);

  auto data = readMeshBinary("resources/cube.obj");
  engine.createMesh(
    material,
    data.first,
    data.second);

  engine.run();
}