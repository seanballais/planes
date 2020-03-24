#include <sstream>
#include <stdexcept>
#include <string>

#include <planes/engine/ecs/component.hpp>
#include <planes/engine/ecs/entity.hpp>
#include <planes/engine/ecs/errors.hpp>
#include <planes/engine/ecs/system.hpp>

namespace planes::engine::ecs
{
  System::System(const Signature signature, ComponentManager& componentManager)
    : signature(signature)
    , componentManager(componentManager)
    , entityToIndexMap({})
    , numEntities(0) {}

  void System::addEntity(const Entity e)
  {
    auto item = this->entityToIndexMap.find(e);
    if (item == this->entityToIndexMap.end()) {
      this->entities.push_back(e);
      this->entityToIndexMap.insert({e, this->numEntities});

      this->numEntities++;
    } else {
      std::stringstream errorMsgStream;
      errorMsgStream << "Attempted to add already added entity, " << e;
      std::string errorMsg = errorMsgStream.str();

      throw EntityAlreadyExistsError(errorMsg);
    }
  }

  void System::removeEntity(const Entity e)
  {
    auto item = this->entityToIndexMap.find(e);
    if (item == this->entityToIndexMap.end()) {
      std::stringstream errorMsgStream;
      errorMsgStream << "Attempted to remove a non-registered entity, " << e;
      std::string errorMsg = errorMsgStream.str();

      throw UnregisteredEntityError(errorMsg);
    } else {
      size_t entityIndex = item->second;      
      const Entity& lastEntity = this->entities.back();
      this->entities[entityIndex] = lastEntity;
      this->entities.pop_back();

      this->entityToIndexMap.erase(item);
    }
  }

  EntityAlreadyExistsError::EntityAlreadyExistsError(const char* what_arg)
    : std::runtime_error(what_arg) {}

  EntityAlreadyExistsError::EntityAlreadyExistsError(const std::string what_arg)
    : std::runtime_error(what_arg) {}

  UnregisteredEntityError::UnregisteredEntityError(const char* what_arg)
    : std::runtime_error(what_arg) {}

  UnregisteredEntityError::UnregisteredEntityError(const std::string what_arg)
    : std::runtime_error(what_arg) {}
}
