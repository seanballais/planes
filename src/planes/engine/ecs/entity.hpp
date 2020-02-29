#ifndef PLANES_ENGINE_ECS_ENTITY_HPP
#define PLANES_ENGINE_ECS_ENTITY_HPP

#include <bitset>
#include <cstdint>
#include <stdexcept>
#include <queue>

#include "component.hpp"

namespace planes::engine::ecs
{
  using Entity = std::uint16_t;
  using Signature = std::bitset<planes::engine::ecs::MAX_NUM_COMPONENTS>;

  template <std::uint16_t maxNumEntities>
  class EntityManager
  {
  public:
    EntityManager();

    Entity createEntity();

    const std::uint16_t kMaxNumEntities = maxNumEntities;
  private:
    Signature entities[maxNumEntities];
    std::queue<Entity> unusedEntities;
  };

  const std::uint16_t kDefaultMaxNumEntities = 10000;
  using DefaultEntityManager = EntityManager<kDefaultMaxNumEntities>();

  class TooManyEntitiesError : public std::runtime_error
  {
  public:
    TooManyEntitiesError(const std::string& what_arg);
  };
}

#endif
