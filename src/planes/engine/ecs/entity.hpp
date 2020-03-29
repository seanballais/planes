#ifndef PLANES_ENGINE_ECS_ENTITY_HPP_
#define PLANES_ENGINE_ECS_ENTITY_HPP_

#include <bitset>
#include <set>
#include <stdexcept>
#include <queue>

#include <planes/engine/ecs/constants.hpp>

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

  using Entity = unsigned int;
  using Signature = std::bitset<planes::engine::ecs::MAX_NUM_COMPONENTS>;

  template<size_t maxNumEntities>
  class EntityManager
  {
  public:
    EntityManager()
    {
      for (Entity e = 0; e < this->kMaxNumEntities; e++) {
        this->unusedEntitiesQueue.push(e);
        this->unusedEntities.insert(e);
        this->entitySignatures[e] = Signature{0};
      }
    }

    Entity createEntity()
    {
      if (unusedEntities.empty()) {
        throw TooManyEntitiesError{"Created too many entities than allowed."};
      }

      Entity entity = this->unusedEntitiesQueue.front();
      this->unusedEntitiesQueue.pop();
      this->unusedEntities.erase(entity);

      return entity;
    }

    void deleteEntity(Entity entity)
    {
      if (!this->isEntityInRange(entity)) {
        throw std::out_of_range{"Attempted to delete an invalid entity."};
      }

      if (!this->doesEntityExist(entity)) {
        throw NonExistentEntityError{
          "Attempted to delete a non-existent entity."};
      }

      this->unusedEntitiesQueue.push(entity);
      this->unusedEntities.insert(entity);
      this->entitySignatures[entity].reset();
    }

    Signature getSignature(Entity entity)
    {
      if (!this->isEntityInRange(entity)) {
        throw std::out_of_range{
          "Attempted to get the signature of an invalid entity."};
      }

      if (!this->doesEntityExist(entity)) {
        throw NonExistentEntityError{
          "Attempted to get the signature of a non-existent entity."};
      }

      return this->entitySignatures[entity];
    }

    void setSignature(Entity entity, Signature signature)
    {
      if (!this->isEntityInRange(entity)) {
        throw std::out_of_range(
          "Attempted to set the signature of an invalid entity.");
      }

      if (!this->doesEntityExist(entity)) {
        throw NonExistentEntityError(
          "Attempted to get the signature of a non-existent entity.");
      }

      this->entitySignatures[entity] = signature;
    }

    const std::uint16_t kMaxNumEntities = maxNumEntities;
  private:
    bool isEntityInRange(Entity entity)
    {
      return entity >= 0 && entity < this->kMaxNumEntities;
    }

    bool doesEntityExist(Entity entity)
    {
      auto it = this->unusedEntities.find(entity);
      return it == this->unusedEntities.end();
    }

    Signature entitySignatures[maxNumEntities];
    std::queue<Entity> unusedEntitiesQueue;
    std::set<Entity> unusedEntities;
  };
}

#endif
