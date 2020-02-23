#ifndef PLANES_ENGINE_ECS_ENTITY_HPP
#define PLANES_ENGINE_ECS_ENTITY_HPP

#include <bitset>
#include <cstdint>

#include "component.hpp"

namespace planes::engine::ecs
{
  using Entity = std::uint16_t;
  using Signature = std::bitset<planes::engine::ecs::MAX_NUM_COMPONENTS>;

  const std::uint16_t MAX_NUM_ENTITIES = 10000;

  class EntityManager
  {
  public:
    EntityManager();

    Entity createEntity();
  private:
    Signature entities[MAX_NUM_ENTITIES];
  }
}

#endif
