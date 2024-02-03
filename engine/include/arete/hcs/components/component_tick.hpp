#ifndef ARETE_COMPONENT_TICK_HPP
#define ARETE_COMPONENT_TICK_HPP

#include "component.hpp"

namespace arete {

//! Behaviour component base.
class ComponentTick : public Component
{
public:
  virtual ~ComponentTick() = default;
  
public:
  virtual void onTick();
};

} // arete

#endif // ARETE_COMPONENT_TICK_HPP