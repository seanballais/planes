#ifndef PLANES_ENGINE_ECS_ENTITY_HPP_
#define PLANES_ENGINE_ECS_ENTITY_HPP_

#include <bitset>
#include <cstdint>
#include <set>
#include <stdexcept>
#include <queue>

#include "component.hpp"

namespace planes::engine::ecs
{
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

  using Entity = std::uint16_t;
  using Signature = std::bitset<planes::engine::ecs::MAX_NUM_COMPONENTS>;

  template<std::uint16_t maxNumEntities>
  class EntityManager
  {
  public:
    const std::uint16_t kMaxNumEntities = maxNumEntities;

    EntityManager()
    {
      for (Entity e = 0; e < kMaxNumEntities; e++) {
        this->unusedEntitiesQueue.push(e);
        this->unusedEntities.insert(e);
        this->entitySignatures[e] = Signature{0};
      }
    }

    Entity createEntity()
    {
      if (unusedEntities.empty()) {
        throw TooManyEntitiesError("Created too many entities than allowed.");
      }

      Entity entity = unusedEntitiesQueue.front();
      unusedEntitiesQueue.pop();
      unusedEntities.erase(entity);

      return entity;
    }

    void deleteEntity(Entity entity)
    {
      if (!isEntityInRange(entity)) {
        throw std::out_of_range("Attempted to delete an invalid entity.");
      }

      if (!doesEntityExist(entity)) {
        throw NonExistentEntityError(
          "Attempted to delete a non-existent entity.");
      }

      unusedEntitiesQueue.push(entity);
      unusedEntities.insert(entity);
      entitySignatures[entity].reset();
    }

    Signature getSignature(Entity entity)
    {
      if (!isEntityInRange(entity)) {
        throw std::out_of_range(
          "Attempted to get the signature of an invalid entity.");
      }

      if (!doesEntityExist(entity)) {
        throw NonExistentEntityError(
          "Attempted to get the signature of a non-existent entity.");
      }

      return entitySignatures[entity];
    }

    void setSignature(Entity entity, Signature signature)
    {
      if (!isEntityInRange(entity)) {
        throw std::out_of_range(
          "Attempted to set the signature of an invalid entity.");
      }

      if (!doesEntityExist(entity)) {
        throw NonExistentEntityError(
          "Attempted to get the signature of a non-existent entity.");
      }

      entitySignatures[entity] = signature;
    }
  private:
    Signature entitySignatures[maxNumEntities];
    std::queue<Entity> unusedEntitiesQueue;
    std::set<Entity> unusedEntities;

    bool isEntityInRange(Entity entity)
    {
      return entity >= 0 && entity < kMaxNumEntities;
    }

    bool doesEntityExist(Entity entity)
    {
      auto it = this->unusedEntities.find(entity);
      return it == this->unusedEntities.end();
    }
  };

  const std::uint16_t kDefaultMaxNumEntities = 10000;
  using DefaultEntityManager = EntityManager<kDefaultMaxNumEntities>();
}

#endif
