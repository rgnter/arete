#include "arete/hcs/actors/camera.hpp"
#include "arete/components/spatial.hpp"

namespace arete {

void Camera::OnCreate()
{
    CreateComponent<SpatialComponent>();
}

void Camera::OnDestroy()
{

}

}