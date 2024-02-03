//
// Created by maros on 3.12.2023.
//

#ifndef ARETE_COMPONENTS_HPP
#define ARETE_COMPONENTS_HPP

#include "arete/core.hpp"
#include "arete/hcs/arena.hpp"

#include <tuple>
#include <cstdint>

namespace arete::hcs
{

//! Component handle.
using ComponentHandle = uint32_t;

//! Component base.
class ComponentBase
{
public:
  //!
  virtual void onCreate() = 0;

  //!
  virtual void onDestroy() = 0;
};

//! Behaviour component base.
class BehaviourComponentBase
  : public ComponentBase
{
public:
  //!
  virtual void onTick() = 0;
};

template<class ComponentHandleType, class ComponentType>
class SystemBase
{
private:
  Arena<ComponentHandleType, ComponentType> _arena;

public:
  Arena<ComponentHandleType, ComponentType>& arena()
  {
    return _arena;
  }
};

} // namespace arete::hcs

#endif//ARETE_COMPONENTS_HPP
