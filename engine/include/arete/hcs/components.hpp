//
// Created by maros on 3.12.2023.
//

#ifndef ARETE_COMPONENTS_HPP
#define ARETE_COMPONENTS_HPP

#include <cstdint>

namespace arete
{

//! Component handle.
using ComponentHandle = uint32_t;

//! Component.
class ComponentBase
{
};

//! Behaviour component.
class ComponentBehaviourBase
{
public:
  virtual void OnTick();
};

}

#endif//ARETE_COMPONENTS_HPP
