#ifndef ARETE_MESH_RENDERER_HPP
#define ARETE_MESH_RENDERER_HPP

#include "arete/structures/structures_common.hpp"

namespace arete::components {

class MeshRenderer : public ComponentDrawable
{

public:
    virtual void onDraw() override;

private:
    MeshHandle _mesh;
    MaterialHandle _material;
    
};

}



#endif // ARETE_MESH_RENDERER_HPP