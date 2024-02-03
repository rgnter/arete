#ifndef ARETE_MODEL_HPP
#define ARETE_MODEL_HPP

#include "arete/structures/structures_common.hpp"

#include <glm/vec3.hpp>
#include <glm/fwd.hpp>

namespace arete
{

//! Model.
class Model
{
private:
  std::vector<MeshHandle> _meshes;
};

} // namespace arete

#endif // ARETE_MODEL_HPP
