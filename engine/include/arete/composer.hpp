//
// Created by maros on 3.2.2024.
//

#ifndef ARETE_COMPOSER_HPP
#define ARETE_COMPOSER_HPP

namespace arete
{

map<>

int test()
{
  while(true)
  {
    _systems.tick();

    RenderCtx x;
    drawSystem.tick(x)

    Composer::doIt(x);
  }
}

// Actor
// -> MeshComponent

std::array<ComponentSystem> _systems;

class MeshComponentSystem : DrawSystem
{
  void tick() {
    ctx = Engine::Get()._composer.currentPass();

    _arena.components();
    ctx.Material
  }
};

}

#endif//ARETE_COMPOSER_HPP
