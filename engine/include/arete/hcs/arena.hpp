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

//! Arena.
template<class ObjectHandleType, class ObjectType>
class Arena
{
public:
  //! Contiguous array of objects.
  using ObjectArray = std::vector<ObjectType>;
  //! Represents index of an object in the array of objects.
  using ObjectIndex = int64_t;
  //! Represents index of all object handles to object indexes.
  using ObjectIndexMap = std::unordered_map<ObjectHandleType, ObjectIndex>;
  //! Represents array of freed objects of which space can be re-used.
  using ObjectIndexFreeList = std::vector<ObjectIndex>;
  //! Atomic counter of unique objects created in this component arena.
  using ObjectCounter = std::atomic<ObjectHandleType>;

  //! Default constructor.
  Arena() = default;

  //! Deleted copy constructor.
  Arena(const Arena& rhs) = delete;

  //! Deleted move constructor.
  Arena(Arena&& rhs) = delete;

  template<typename... _Args>
  //! Creates new object in the arena.
  //! @returns Pair of object handle and object reference.
  [[nodiscard]] std::pair<ObjectHandleType, ObjectType&> createObject(_Args&&... args) {
    const ObjectHandleType componentHandle = _objectCounter++;

    // Find an empty space for this object.
    // Try to get last element from the free list if available,
    // otherwise continue in contiguous allocations.
    ObjectIndex index = -1;
    if (!_objectFreelist.empty())
    {
      index = _objectFreelist.back();
      _objectFreelist.pop_back();
      _objects[index] = std::move(ObjectType{std::forward<_Args>(args)...});
    } else
    {
      index = _objects.size();
      _objects.emplace_back(std::forward<_Args>(args)...);
    }

    auto& object = _objects[index];


    // Index the object handle to the index.
    _objectIndex[componentHandle] = index;

    return {componentHandle, object};
  }

  //! Destroys object in the arena.
  //! @param objectHandle Object handle.
  [[maybe_unused]] bool destroyObject(const ObjectHandleType objectHandle)
  {
    auto indexIterator = _objectIndex.find(objectHandle);
    if (indexIterator == _objectIndex.end())
      return false;

    // Get the index assigned to the object handle.
    ObjectIndex index = indexIterator->first;

    // Erase the object index to object handle index entry
    // and mark this component index as free.
    _objectIndex.erase(indexIterator);
    _objectFreelist.push_back(index);

    return _objectIndex.erase(objectHandle);
  }

  //! Gets object by its handle.
  //! @returns Optional object reference.
  [[nodiscard]] std::optional<Ref<ObjectType>> getObject(const ObjectHandleType objectHandle)
  {
    auto indexIterator = _objectIndex.find(objectHandle);
    if (indexIterator == _objectIndex.end())
      return std::nullopt;
    return _objects[indexIterator->second];
  }

  //! @returns Reference to contiguous array of objects.
  [[nodiscard]] ObjectArray& objects() noexcept
  {
    return _objects;
  }

  //! @returns Reference to immutable contiguous array of objects.
  [[nodiscard]] const ObjectArray& objects() const noexcept
  {
    return _objects;
  }

private:
  ObjectArray _objects;
  ObjectIndexMap _objectIndex;
  ObjectIndexFreeList _objectFreelist;

  ObjectCounter _objectCounter{ 0 };
};

} // namespace arete::hcs

#endif //ARENA_HPP
