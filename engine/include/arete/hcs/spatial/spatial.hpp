#ifndef SPATIAL_HPP
#define SPATIAL_HPP

#include "arete/hcs/hcs.hpp"

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

namespace arete::hcs
{

//! Spatial component.
class SpatialComponent
  : public BehaviourComponentBase {

public:
  inline virtual void HandleInput(float value, int key);

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
