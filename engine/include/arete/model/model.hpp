#ifndef ARETE_MESH_HPP
#define ARETE_MESH_HPP

#include <cstdint>

#include <vector>

#include <glm/vec3.hpp>
#include <glm/fwd.hpp>

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

    //! Constructs shader with source which is bound to specified stage.
    //! @param stage Shader stage in the pipeline.
    //! @param source Shader source.
    explicit Shader(
      const Stage stage,
      std::vector<uint8_t> source) noexcept
      : _stage(stage)
      , _source(std::move(source))
    {}

    //! @returns Stage to which this shader is bound.
    const Stage& stage() const
    {
      return _stage;
    }

    //! @returns Source of this shader.
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
    //! @param vertexShader Vertex shader.
    //! @param fragmentShader Fragment shader.
    explicit Material(
      const ShaderHandle vertexShader,
      const ShaderHandle fragmentShader) noexcept
        : _vertexShader(vertexShader)
        , _fragmentShader(fragmentShader)
    {}

    //! @returns Vertex shader handle.
    ShaderHandle vertexShader() const
    {
      return _vertexShader;
    }

    //! @returns Fragment shader handle.
    ShaderHandle fragmentShader() const
    {
      return _fragmentShader;
    }

  private:
    ShaderHandle _vertexShader;
    ShaderHandle _fragmentShader;
  };

  //! Mesh handle.
  using MeshHandle = uint32_t;

  //! Mesh.
  class Mesh
  {
  public:
    //! Single vertex element type.
    using VertexElementType = glm::f32vec3;
    //! Verticies.
    using Vertices = std::vector<VertexElementType>;

    //! Single index element type.
    using IndexElementType = glm::u16vec3;
    //! Indices.
    using Indices = std::vector<IndexElementType>;

    //! Constructs model with specified material and model data.
    //! @param material Material handle.
    //! @param vertices Mesh vertex data.
    //! @param indices Mesh index data.
    explicit Mesh(MaterialHandle material,
                  Vertices vertices,
                  Indices indices) noexcept
      : _material(material)
      , _vertices(std::move(vertices))
      , _indices(std::move(indices))
    {}

  private:
    MaterialHandle _material;

    Vertices _vertices;
    Indices _indices;
  };

  //! Model.
  class Model
  {
  private:
    std::vector<MeshHandle> _meshes;
  };

} // namespace arete

#endif//ARETE_MESH_HPP
