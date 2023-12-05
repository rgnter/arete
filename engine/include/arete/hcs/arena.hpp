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
  using ComponentIndex = int64_t;
  using ComponentArray = std::vector<ComponentType>;
  using ComponentIndexMap = std::unordered_map<ComponentHandleType, ComponentIndex>;
  using ComponentFreelist = std::vector<ComponentIndex>;

  using ComponentCounter = std::atomic<ComponentHandleType>;

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
    // and use that to retrieve reference to component.
    ComponentIndex index = indexIterator->first;

    // Erase the component index to component handle assignment
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
