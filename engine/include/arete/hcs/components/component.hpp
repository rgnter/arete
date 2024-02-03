#ifndef ARETE_COMPONENT_HPP
#define ARETE_COMPONENT_HPP

namespace arete {

//! Data component base.
class Component
{
public:
  virtual void onCreate();
  virtual void onDelete();
};

} // arete


#endif // ARETE_COMPONENT_HPP