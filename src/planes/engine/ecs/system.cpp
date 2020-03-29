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
      errorMsgStream << "Attempted to add already added entity, "
                     << e << ".";
      std::string errorMsg = errorMsgStream.str();

      throw EntityAlreadyExistsError(errorMsg);
    }
  }

  void System::removeEntity(const Entity e)
  {
    auto item = this->entityToIndexMap.find(e);
    if (item == this->entityToIndexMap.end()) {
      std::stringstream errorMsgStream;
      errorMsgStream << "Attempted to remove a non-registered entity, "
                     << e << ".";
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

  bool System::hasEntity(const Entity e) const
  {
    return this->entityToIndexMap.find(e) != this->entityToIndexMap.end();
  }

  Signature System::getSignature() const
  {
    return this->signature;
  }

  SystemManager::SystemManager(ComponentManager& componentManager)
    : componentManager(componentManager) {}

  void SystemManager::addEntity(const Entity e, const Signature signature)
  {
    for (const auto& item : this->nameToSystem) {
      System* system = item.second.get();
      if ((signature & system->getSignature()) == system->getSignature()) {
        // Entity has a signature that is compatible with the system's
        // signature. They are compatible since the entity has *all* the
        // components required by the system. This checking mechanism works
        // like this:
        //
        // Applying an AND to a set of bits with itself will give the same value
        // as the set. Applying an AND to two different sets of bits, where
        // the second operand set is a subset of the first operand set, will
        // give the value of the second operand. Having the second operand set
        // be a subset of the first, in this context, means that the two sets
        // are compatible. This is how the compatibility check mechanism works.
        system->addEntity(e);
      }
    }
  }

  void SystemManager::removeEntity(const Entity e)
  {
    for (const auto& item : this->nameToSystem) {
      System* system = item.second.get();
      if (system->hasEntity(e)) {
        system->removeEntity(e);
      }
    }
  }

  void SystemManager::notifyEntitySignatureChanged(Entity e,
                                                   const Signature signature)
  {
    for (const auto& item : this->nameToSystem) {
      System* system = item.second.get();
      if (system->hasEntity(e)
          && ((signature & system->getSignature()) != system->getSignature())) {
        system->removeEntity(e);
      } else {
        if ((signature & system->getSignature()) == system->getSignature()) {
          // Entity has a signature that is compatible with the system's
          // signature. They are compatible since the entity has *all* the
          // components required by the system. This checking mechanism works
          // like this:
          //
          // Applying an AND to a set of bits with itself will give the same
          // value as the set. Applying an AND to two different sets of bits,
          // where the second operand set is a subset of the first operand set,
          // will give the value of the second operand. Having the second
          // operand set be a subset of the first, in this context, means that
          // the two sets are compatible. This is how the compatibility check
          // mechanism works.
          system->addEntity(e);
        }
      }
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

  UnregisteredSystemError::UnregisteredSystemError(const char* what_arg)
    : std::runtime_error(what_arg) {}

  UnregisteredSystemError::UnregisteredSystemError(const std::string what_arg)
    : std::runtime_error(what_arg) {}
}
