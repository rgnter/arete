#ifndef ARETE_MESH_HPP
#define ARETE_MESH_HPP

#include <cstdint>

#include <vector>
#include <unordered_map>

namespace arete
{
  //! Shader handle.
  using ShaderHandle = uint32_t;

  //! Shader.
  class Shader {
  public:
    //! Shader stage.
    enum class Stage {
      Vertex, Fragment
    };

  public:
    //! Constructs shader with source which is bound to specified stage.
    //! @param stage Shader stage in the pipeline.
    //! @param source Shader source.
    explicit Shader(Stage stage,
                   std::vector<uint8_t> source) noexcept
      : _stage(stage)
      , _source(std::move(source))
    {}

    Stage stage() const
    {
      return _stage;
    }

    const std::vector<uint8_t>& source() const
    {
      return _source;
    }

  private:
    //! Stage.
    Stage _stage;
    //! Source.
    std::vector<uint8_t> _source;
  };

  //! Material handle.
  using MaterialHandle = uint32_t;

  //! Material.
  class Material
  {
  public:
    //! Constructs material with specified shaders.
    //! @param shaders Shader handles.
    explicit Material(std::vector<ShaderHandle> shaders) noexcept
        : _shaders(std::move(shaders))
    {}

  private:
    std::vector<ShaderHandle> _shaders;
  };

  //! Mesh handle.
  using MeshHandle = uint32_t;

  //! Mesh.
  class Mesh
  {
  public:
    //! Constructs model with specified material and model data.
    //! @param material Material handle.
    //! @param data Mesh data.
    explicit Mesh(MaterialHandle material,
                  std::vector<uint16_t> data) noexcept
      : _material(material)
      , _data(std::move(data))
    {}

  private:
    MaterialHandle _material;
    std::vector<uint16_t> _data;
  };

  //! Model.
  class Model
  {
  private:
    std::vector<MeshHandle> _meshes;
  };

}

#endif//ARETE_MESH_HPP
