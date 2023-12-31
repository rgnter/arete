#ifndef SPATIAL_HPP
#define SPATIAL_HPP

#include "component_data.hpp"

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

namespace arete::hcs
{

//! Spatial component.
class SpatialComponent
  : public ComponentDataBase {

public:
  SpatialComponent& translate();
  SpatialComponent& rotate();

private:
  glm::vec3 _position;
  glm::quat _rotation;
  glm::vec3 _scale;
};

} // namespace arete::hcs



#endif //SPATIAL_HPP
