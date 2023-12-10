#idndef ARETE_COMPONENT_BEHAVIOUR_HPP
#define ARETE_COMPONENT_BEHAVIOUR_HPP

namespace arete {

//! Behaviour component base.
class ComponentBehaviourBase
{
public:
  virtual ~ComponentBehaviourBase() = default;
  
public:
  virtual void onCreate();
  virtual void onDelete();

  virtual void onTick();
};

}

#endif ARETE_COMPONENT_BEHAVIOUR_HPP