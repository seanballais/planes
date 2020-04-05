#include <planes/engine/ecs/ecs.hpp>

namespace planes::engine::ecs
{
  ECS::ECS()
    : entityManager()
    , componentManager()
    , systemManager(componentManager) {}

  Entity ECS::createEntity()
  {
    return this->entityManager.createEntity();
  }

  void ECS::deleteEntity(const Entity e)
  {
    this->entityManager.deleteEntity(e);
    this->componentManager.notifyEntityDeleted(e);
    this->systemManager.removeEntityFromSystems(e);
  }
}
