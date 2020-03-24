#ifndef PLANES_ENGINE_ECS_SYSTEM_HPP_
#define PLANES_ENGINE_ECS_SYSTEM_HPP_

#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#include <planes/engine/ecs/component.hpp>
#include <planes/engine/ecs/entity.hpp>

namespace planes::engine::ecs
{
  class EntityAlreadyExistsError;
  class UnregisteredEntityError;

  class System
  {
  public:
    System(const Signature signature, ComponentManager& componentManager);
    void addEntity(const Entity e);
    void removeEntity(const Entity e);

    virtual void update() = 0;

  protected:
    const Signature signature;
    ComponentManager& componentManager;

    // We're using a vector here for cache locality during system updates.
    std::vector<Entity> entities;

  private:
    // This unordered set is typically used during entity addition and removal
    // for a fast checking if an entity is registered in the system or not.
    std::unordered_map<Entity, size_t> entityToIndexMap;

    size_t numEntities;
  };

  class EntityAlreadyExistsError : public std::runtime_error
  {
  public:
    EntityAlreadyExistsError(const char* what_arg);
    EntityAlreadyExistsError(const std::string what_arg);
  };

  class UnregisteredEntityError : public std::runtime_error
  {
  public:
    UnregisteredEntityError(const char* what_arg);
    UnregisteredEntityError(const std::string what_arg);
  };
}

#endif
