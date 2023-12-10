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
    [[nodiscard]] const Stage& stage() const
    {
      return _stage;
    }

    //! @returns Source of this shader.
    [[nodiscard]] const std::vector<uint8_t>& source() const
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
    [[nodiscard]] ShaderHandle vertexShader() const
    {
      return _vertexShader;
    }

    //! @returns Fragment shader handle.
    [[nodiscard]] ShaderHandle fragmentShader() const
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

    [[nodiscard]] MaterialHandle material() const
    {
      return _material;
    }

    //! @returns Reference to vertices.
    [[nodiscard]] const Vertices& vertices() const
    {

      return _vertices;
    }

    //! @returns Reference to indices.
    [[nodiscard]] const Indices& indices() const
    {
      return _indices;
    }

    static const Vertices getCubeVertices()
    {
      return {
        // FRONT
        {-0.5f, -0.5f, +0.5f},
        {+0.5f, -0.5f, +0.5f},
        {-0.5f, +0.5f, +0.5f},
        {+0.5f, +0.5f, +0.5f},

        // BACK
        {-0.5f, -0.5f, -0.5f},
        {+0.5f, -0.5f, -0.5f},
        {-0.5f, +0.5f, -0.5f},
        {+0.5f, +0.5f, -0.5f}
      };
    }

    static const Indices getCubeIndices()
    {
      return {
        // FRONT
        // FRONT bottom-left
        {2, 1, 0},
        // FRONT upper-right
        {1, 2, 3},

        // BACK
        {6, 4, 5},
        // BACK
        {5, 7, 6},

        // RIGHT
        {3, 5, 1},
        // RIGHT
        {5, 3, 7},

        // LEFT
        {6, 0, 4},
        // LEFT
        {0, 6, 2},

        // TOP
        {6, 3, 2},
        // TOP
        {3, 6, 7},

        // BOTTOM
        {0, 5, 4},
        // BOTTOM
        {5, 0, 1},
      };
    }

    static const Vertices getPlaneVertices()
    {
      return {
        // FRONT
        {-0.5f, 0, -0.5f},
        {-0.5f, 0, +0.5f},
        {+0.5f, 0, -0.5f},
        {+0.5f, 0, +0.5f},
      };
    }

    static const Indices getPlaneIndices()
    {
      return {
        {2, 1, 0},
        {1, 2, 3},
      };
    }


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
