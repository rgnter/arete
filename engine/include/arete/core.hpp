#ifndef ARETE_CORE_HPP
#define ARETE_CORE_HPP

#include "model/model.hpp"
#include "arete/input/input.hpp"

#include <memory>
#include <utility>

namespace arete
{

//! Simple reference wrapper.
template<class T>
using Ref = std::reference_wrapper<T>;

} // namespace arete

#endif //ARETE_CORE_HPP
