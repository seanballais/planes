#include <sstream>

#include <planes/engine/ecs/entity.hpp>
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
    this->checkIfEntityExists(e);

    this->entityManager.deleteEntity(e);
    this->componentManager.notifyEntityDeleted(e);
    this->systemManager.removeEntityFromSystems(e);
  }

  void ECS::checkIfEntityExists(const Entity e)
  {
    if (!this->entityManager.doesEntityExist(e)) {
      std::stringstream errorMsgStream;
      errorMsgStream << "Entity, " << e << ", does not exist.";
      const std::string errorMsg = errorMsgStream.str();
      throw NonExistentEntityError{errorMsg};
    }
  }
}
