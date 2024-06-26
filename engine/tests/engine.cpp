#include <arete/vulkan.hpp>
#include <fstream>
#include <filesystem>
#include <format>

int main(int argc, char** argv)
{
  const auto readMeshBinary = [](const std::filesystem::path& meshBinaryPath)
  {
    std::ifstream input(meshBinaryPath, std::ios::binary);
    if (!input.is_open())
      throw std::runtime_error(""
        /*std::format("Couldn't find mesh at '{}'", meshBinaryPath.c_str())*/);

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
    readSpvBinary("resources/shaders/cube-vertex.spv"));
  auto fragmentShader = engine.createShader(
    arete::Shader::Stage::Fragment,
    readSpvBinary("resources/shaders/cube-fragment.spv"));

  auto material = engine.createMaterial(
    vertexShader,
    fragmentShader);

  //auto data = readMeshBinary("resources/cube.obj");
  
  auto cubeMesh = engine.createMesh(
    material,
    arete::Mesh::getCubeVertices(),
    arete::Mesh::getCubeIndices()
  );

  // arete::Mesh::Vertices planeVerts = arete::Mesh::getPlaneVertices();
  // for (auto & vertex : planeVerts)
  // {
  //   vertex -= arete::Mesh::VertexElementType(0, 1, 0);
  // }
  // auto planeMesh = engine.createMesh(
  //   material,
  //   planeVerts,
  //   arete::Mesh::getPlaneIndices()
  // );


  /*
    //! Player input component.
    //! Controls simple spatial transformations based on inputs.
    class PlayerInputComponent
        : arete::hcs::InputComponent
    {
    public:
      void OnCreate(arete::Scene& scene) override
      {
        _spatial = GetActor().GetComponent<arete::hcs::SpatialComponent>();
      }

      void OnDestroy(arete::Scene& scene) override
      {
      }

      void HandleInput(float value, arete::InputKey key) override
      {
        glm::vec3 axis;
        if (key == arete::InputKey::Keyboard_Right)
          axis = {1, 0, 0};
        else if (key == arete::InputKey::Keyboard_Left)
          axis = {-1, 0, 0};
        else if (key == arete::InputKey::Keyboard_Upward)
          axis = {0, 0, 1};
        else if (key == arete::InputKey::Keyboard_Downward)
          axis = {0, 0, -1};

        _spatial.translate(axis * value);
      }

    private:
      arete::Ref<arete::hcs::SpatialComponent> _spatial;

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
      arete::Ref<arete::hcs::MeshComponent> _mesh;
      arete::Ref<arete::hcs::SpatialComponent> _spatial;
      arete::Ref<arete::hcs::InputComponent> _input;
    };*/

  engine.run();
}