#ifndef ARETE_COMPONENT_DRAW_HPP
#define ARETE_COMPONENT_DRAW_HPP

#include "component.hpp"

namespace arete {

//! Behaviour component base.
class ComponentDraw : public Component
{
public:
  virtual ~ComponentDraw() = default;
  
public:
  virtual void onDraw();
};

}

#endif // ARETE_COMPONENT_DRAW_HPP