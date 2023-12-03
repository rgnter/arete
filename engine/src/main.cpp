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
  auto mesh = engine.createMesh(
    material,
    arete::Mesh::Vertices{
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

  /*
  //! Player input component.
  //! Simple spatial translations.
  class PlayerInputComponent
      : arete::InputComponent
  {
  public:
    void Handle2DInput(float value)
    {
      
    }

    void Handle1DInput(float value, int64_t keyId) override
    {
      glm::vec3 axis;
      if (value == INPUT_RIGHT)
        axis = {1, 0, 0};
      if (value == INPUT_LEFT)
        axis = {-1, 0, 0};
      if (value == INPUT_FORWARD)
        axis = {0, 0, 1};
      if (value == INPUT_BACKWARD)
        axis = {0, 0, -1};

      _spatial.translate(axis * value);
    }

  private:
    arete::SpatialComponent _spatial;

  };

  //! Player actor.
  class Player
      : arete::Actor
  {
  public:
    void OnCreate(Scene& scene) override
    {
      // Create the mesh component.
      _mesh = CreateComponent<arte::MeshComponent>(
        arete::assets::find("primitives/cube.fbx"),
        arete::assets::find("primitives/materials/cube.mat"));

      // Create the spatial component.
      _spatial = CreateComponent<arete::SpatialComponent>();

      // Create the input component.
      _input = CreateComponent<PlayerInputComponent>(_spatial);
    }

    void OnDestroy() override
    {
    }

  private:
    arete::Ref<arete::MeshComponent> _mesh;
    arete::Ref<arete::SpatialComponent> _spatial;
    arete::Ref<arete::InputComponent> _input;
  };
*/

  engine.run();
}