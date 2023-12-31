#ifndef ARENA_HPP
#define ARENA_HPP

#include "arete/core.hpp"

#include <atomic>
#include <cstdint>
#include <utility>
#include <optional>
#include <functional>
#include <unordered_map>

namespace arete::hcs
{

//! System component arena.
template<class ComponentHandleType, class ComponentType>
class ComponentArena
{
public:
  //! Contiguous array of components.
  using ComponentArray = std::vector<ComponentType>;
  //! Represents index of an component in the array of components.
  using ComponentIndex = int64_t;
  //! Represents index of all component handles to component indexes.
  using ComponentIndexMap = std::unordered_map<ComponentHandleType, ComponentIndex>;
  //! Represents array of freed components of which space can be re-used.
  using ComponentFreelist = std::vector<ComponentIndex>;
  //! Atomic counter of unique components created in this component arena.
  using ComponentCounter = std::atomic<ComponentHandleType>;

  //! Default constructor.
  ComponentArena() = default;

  //! Deleted copy constructor.
  ComponentArena(const ComponentArena& rhs) = delete;

  //! Deleted move constructor.
  ComponentArena(ComponentArena&& rhs) = delete;

  //! Creates new component in the arena.
  //! @returns Pair of component handle and component reference.
  [[nodiscard]] std::pair<ComponentHandleType, ComponentType&> createComponent() {
    const ComponentHandleType componentHandle = _componentCounter++;

    // Find an empty space for this component.
    // Try to get last element from the free list if available,
    // otherwise continue in contiguous allocations.
    ComponentIndex index = -1;
    if (!_componentFreelist.empty())
    {
      index = _componentFreelist.back();
      _componentFreelist.pop_back();
    } else
    {
      index = _components.size();
      _components.emplace_back();
    }

    auto& component = _components[index];

    // Index the component handle to the index.
    _componentIndex[componentHandle] = index;

    return {componentHandle, component};
  }

  //! Destroys component in the arena.
  //! @param componentHandle ComponentType handle.
  [[maybe_unused]] bool destroyComponent(const ComponentHandleType componentHandle)
  {
    auto indexIterator = _componentIndex.find(componentHandle);
    if (indexIterator == _componentIndex.end())
      return false;

    // Get the index assigned to the component handle
    // and use that to retrieve reference to component and default construct it.
    ComponentIndex index = indexIterator->first;
    _components[index] = {};

    // Erase the component index to component handle index entry
    // and mark this component index as free.
    _componentIndex.erase(indexIterator);
    _componentFreelist.push_back(index);

    return _componentIndex.erase(componentHandle);
  }

  //! Gets component by its handle.
  //! @returns Optional component reference.
  [[nodiscard]] std::optional<Ref<ComponentType>> getComponent(const ComponentHandleType componentHandle)
  {
    auto indexIterator = _componentIndex.find(componentHandle);
    if (indexIterator == _componentIndex.end())
      return std::nullopt;
    return _components[indexIterator->second];
  }

  //! @returns Reference to indexed components.
  [[nodiscard]] ComponentArray& components() noexcept
  {
    return _components;
  }

  //! @returns Reference to immutable indexed components.
  [[nodiscard]] const ComponentArray& components() const noexcept
  {
    return _components;
  }

private:
  ComponentArray _components;
  ComponentIndexMap _componentIndex;
  ComponentFreelist _componentFreelist;

  ComponentCounter _componentCounter{ 0 };
};

} // namespace arete::hcs

#endif //ARENA_HPP
