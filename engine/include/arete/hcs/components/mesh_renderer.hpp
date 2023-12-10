#ifndef ARETE_MESH_RENDERER_HPP
#define ARETE_MESH_RENDERER_HPP

#include "arete/core/core.hpp"

namespace arete::components {

class MeshRenderer
{

private:
    std::vector<MeshHandle> _meshes;
    std::vector<MaterialHandle> _materials;
    
};

}



#endif // ARETE_MESH_RENDERER_HPP