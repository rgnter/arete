#ifndef ARETE_ACTOR_HPP
#define ARETE_ACTOR_HPP

namespace arete {

class Actor
{
public:
  template<typename Component>
  Component * CreateComponent();

  template<typename Component>
  Component * GetComponent();

public:
  //!
  virtual void OnCreate();

  //!
  virtual void OnDestroy();
};

}

#endif ARETE_ACTOR_HPP