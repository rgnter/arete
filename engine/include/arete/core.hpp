#ifndef ARETE_CORE_HPP
#define ARETE_CORE_HPP

#include "model/model.hpp"
#include "arete/input/input.hpp"

namespace arete
{

//! Actor handle.
using ActorHandle = uint32_t;

class Actor
{
public:
  void CreateComponent();

public:
  //!
  virtual void OnCreate();

  //!
  virtual void OnDestroy();
};


class System
{

public:
  virtual void Tick();

};

} // namespace arete

#endif //ARETE_CORE_HPP
