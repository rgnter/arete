//
// Created by maros on 3.12.2023.
//

#ifndef ARETE_COMPONENTS_HPP
#define ARETE_COMPONENTS_HPP

#include "input/input.hpp"
#include "spatial/spatial.hpp"

#include <cstdint>
#include <vector>

namespace arete
{

namespace hcs
{

//! Component handle.
using ComponentHandle = uint32_t;

//! Component base.
class ComponentBase
{
};

//! Behaviour component base.
class ComponentBehaviourBase
{
public:
  virtual ~ComponentBehaviourBase();

public:
  virtual void onTick();
};

//! System handle.
using SystemHandle = uint32_t;

//! System component arena.
template<class Component>
class SystemComponentArena
{

  using Components = std::vector<Component>;

public:
  //!
  [[nodiscard]] Components& components() noexcept
  {
    return _data;
  }

  //!
  [[nodiscard]] const Components& components() const noexcept
  {
    return _data;
  }

private:
  Components _data;
};

//! System base.
template<class Component>
class SystemBase
{
private:
  SystemComponentArena<Component> _arena;
};

//! Actor handle.
using ActorHandle = uint32_t;

class Actor
{
public:
  template<typename Component>
  void CreateComponent();

  template<typename Component>
  void GetComponent();

public:
  //!
  virtual void OnCreate();

  //!
  virtual void OnDestroy();
};

} // namespace hcs

} // namespace arete

#endif//ARETE_COMPONENTS_HPP
