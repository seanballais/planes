#ifndef PLANES_ENGINE_ECS_ENTITY_HPP
#define PLANES_ENGINE_ECS_ENTITY_HPP

#include <bitset>
#include <cstdint>
#include <set>
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
    void deleteEntity(Entity entity);
    Signature getSignature(Entity entity);
    void setSignature(Entity entity, Signature signature);

    const std::uint16_t kMaxNumEntities = maxNumEntities;
  private:
    bool isEntityInRange(Entity entity);
    bool doesEntityExist(Entity entity);

    Signature entitySignatures[maxNumEntities];
    std::deque<Entity> unusedEntitiesQueue;
    std::set<Entity> unusedEntities;
  };

  const std::uint16_t kDefaultMaxNumEntities = 10000;
  using DefaultEntityManager = EntityManager<kDefaultMaxNumEntities>();

  class TooManyEntitiesError : public std::runtime_error
  {
  public:
    TooManyEntitiesError(const char* what_arg);
  };

  class NonExistentEntityError : public std::runtime_error
  {
  public:
    NonExistentEntityError(const char* what_arg);
  };
}

#endif
