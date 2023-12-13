#ifndef ARETE_CAMERA_HPP
#define ARETE_CAMERA_HPP

#include "actor.hpp"

namespace arete {

class Camera : public Actor
{
public:
  //!
  virtual void OnCreate() override;

  //!
  virtual void OnDestroy() override;
};

}

#endif ARETE_ACTOR_HPP