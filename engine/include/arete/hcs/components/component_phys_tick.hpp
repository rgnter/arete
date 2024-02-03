#ifndef ARETE_COMPONENT_PHYS_TICK_HPP
#define ARETE_COMPONENT_PHYS_TICK_HPP

#include "component.hpp"

namespace arete {

//! Behaviour component base.
class ComponentPhysTick : public Component
{
public:
  virtual ~ComponentPhysTick() = default;
  
public:
  virtual void onPhysTick();
};

} // arete

#endif // ARETE_COMPONENT_PHYS_TICK_HPP