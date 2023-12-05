//
// Created by maros on 3.12.2023.
//

#ifndef ARETE_COMPONENTS_HPP
#define ARETE_COMPONENTS_HPP

#include "arete/core.hpp"
#include "arete/hcs/arena.hpp"

#include <cstdint>
#include <atomic>
#include <vector>
#include <unordered_map>

namespace arete::hcs
{

//! ComponentType handle.
using ComponentHandle = uint32_t;

//! ComponentType base.
class ComponentBase
{
public:
  virtual void onCreate();
  virtual void onDelete();
};

//! Behaviour component base.
class BehaviourComponentBase
  : public ComponentBase
{
public:
  virtual ~BehaviourComponentBase() = default;

public:
  virtual void onTick();
};

//! System handle.
using SystemHandle = uint32_t;


template<typename T>
concept Componentable = requires(T a)
{
  { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

//! System base.
template<Componentable Component>
class SystemBase
{
protected:
  ComponentArena<ComponentHandle, Component> _arena;

public:
  //! Creates a component.
  //! @returns Handle of that component.
  [[nodsicard]] ComponentHandle createComponent()
  {
    const auto& [handle, ref] = _arena.createComponent();
    return handle;
  }

  //! Destroys a component.
  //! @param component ComponentType handle.
  virtual void destroyComponent(ComponentHandle component) = 0;

  //! Gets component.
  //! @returns Reference to component.
  //! @throws std::runtime_exception if no such component is created.
  [[nodiscard]] virtual Component& getComponent(ComponentHandle component) = 0;

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

} // namespace arete::hcs

#endif//ARETE_COMPONENTS_HPP
