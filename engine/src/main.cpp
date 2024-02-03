#include <cstdio>

#include <arete/vulkan.hpp>
#include <fstream>
#include <filesystem>
#include <format>

int main(int argc, char** argv)
{
  const auto readMeshBinary = [](const std::filesystem::path& shaderBinaryPath)
  {
    std::ifstream input(shaderBinaryPath, std::ios::binary);
    if (!input.is_open())
      throw std::runtime_error(""
        /*std::format("Couldn't find mesh at '{}'", shaderBinaryPath.c_str())*/);

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
      throw std::runtime_error(""
        /*std::format("Couldn't find shader at '{}'", shaderBinaryPath.c_str())*/);

    std::vector<uint8_t> buffer(size);
    input.read(reinterpret_cast<char*>(buffer.data()), size);
    return buffer;
  };

  vulkan::VulkanEngine engine;

  auto vertexShader = engine.createShader(
    arete::Shader::Stage::Vertex,
    readSpvBinary("resources/shaders/cube-vertex.spv"));
  auto fragmentShader = engine.createShader(
    arete::Shader::Stage::Fragment,
    readSpvBinary("resources/shaders/cube-fragment.spv"));

  auto material = engine.createMaterial(
    vertexShader,
    fragmentShader);

  auto mesh = engine.createMesh(
    material,
    arete::Mesh::Vertices {
      // FRONT
       {-0.5f, -0.5f, +0.5f},
       {+0.5f, -0.5f, +0.5f},
       {-0.5f, +0.5f, +0.5f},
       {+0.5f, +0.5f, +0.5f},

       // BACK
       {-0.5f, -0.5f, -0.5f},
       {+0.5f, -0.5f, -0.5f},
       {-0.5f, +0.5f, -0.5f},
       {+0.5f, +0.5f, -0.5f},

       // RIGHT
       {+0.5f, -0.5f, -0.5f},
       {+0.5f, -0.5f, +0.5f},
       {+0.5f, +0.5f, -0.5f},
       {+0.5f, +0.5f, +0.5f},

       // LEFT
       {-0.5f, -0.5f, -0.5f},
       {-0.5f, -0.5f, +0.5f},
       {-0.5f, +0.5f, -0.5f},
       {-0.5f, +0.5f, +0.5f},

       // TOP
       {-0.5f, +0.5f, -0.5f},
       {+0.5f, +0.5f, -0.5f},
       {-0.5f, +0.5f, +0.5f},
       {+0.5f, +0.5f, +0.5f},

       // BOTTOM
       {-0.5f, -0.5f, -0.5f},
       {+0.5f, -0.5f, -0.5f},
       {-0.5f, -0.5f, +0.5f},
       {+0.5f, -0.5f, +0.5f},
    },
    arete::Mesh::Indices {
      // FRONT bottom-left
        {2, 1, 0},
        // FRONT upper-right
        {1, 2, 3},

        // BACK
        {6, 4, 5},
        // BACK
        {5, 7, 6},

        // RIGHT
        {10, 8, 9},
        // RIGHT
        {9, 11, 10},

        // LEFT
        {14, 13, 12},
        // LEFT
        {13, 14, 15},

        // TOP
        {18, 16, 17},
        // TOP
        {17, 19, 18},

        // BOTTOM
        {22, 21, 20},
        // BOTTOM
        {21, 22, 23}
    });


                    engine.run();
}

class Scene
{
private:
  std::tuple<arete::MeshComponentSystem, arete::SpatialComponentSystem> _systems;
};

class MyActor
    : public arete::Actor
{
private:
  arete::Ref<arete::MeshComponent> _mesh;
  arete::Ref<arete::SpatialComponent> _spatial;

public:
  void onCreate(arete::Scene& scene) override
  {
    _mesh = createComponent<arete::MeshComponent>(scene);
    _spatial = createComponent<arete::SpatialComponent>(scene);
  }

  void onDestroy(arete::Scene& scene) override
  {
    destroyComponent<>(scene, _spatial);
    destroyComponent<>(scene, _mesh);
  }


  template<class ComponentType>
  arete::Ref<ComponentType> createComponent()
  {

  }
};