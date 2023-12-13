#ifndef ARETE_MESH_HPP
#define ARETE_MESH_HPP

#include "structures_common.hpp"

#include <cstdint>

#include <glm/vec3.hpp>
#include <glm/fwd.hpp>

namespace arete
{

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
    : _vertices(std::move(vertices))
    , _indices(std::move(indices))
  {}

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
  Vertices _vertices;
  Indices _indices;
};

} // namespace arete

#endif // ARETE_MESH_HPP
