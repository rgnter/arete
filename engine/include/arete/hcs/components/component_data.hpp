#idndef ARETE_COMPONENT_DATA_HPP
#define ARETE_COMPONENT_DATA_HPP

namespace arete {

//! Data component base.
class ComponentDataBase
{
public:
  virtual void onCreate();
  virtual void onDelete();
};

}


#endif ARETE_COMPONENT_DATA_HPP