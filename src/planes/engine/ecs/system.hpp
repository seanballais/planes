#ifndef PLANES_ENGINE_ECS_SYSTEM_HPP_
#define PLANES_ENGINE_ECS_SYSTEM_HPP_

#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <planes/engine/ecs/component.hpp>
#include <planes/engine/ecs/entity.hpp>

namespace planes::engine::ecs
{
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

  class UnregisteredSystemError : public std::runtime_error
  {
  public:
    UnregisteredSystemError(const char* what_arg);
    UnregisteredSystemError(const std::string what_arg);
  };

  class System
  {
  public:
    System(const Signature signature, ComponentManager& componentManager);
    void addEntity(const Entity e);
    void removeEntity(const Entity e);
    bool hasEntity(const Entity e) const;
    Signature getSignature() const;

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

  class SystemManager
  {
  public:
    SystemManager(ComponentManager& componentManager);
    void addEntity(const Entity e, const Signature signature);
    void removeEntity(const Entity e);
    void notifyEntitySignatureChanged(Entity e, const Signature signature);

    template <class T>
    void registerSystem()
    {
      const std::string systemName = typeid(T).name();
      this->nameToSystem.insert({
        systemName, std::make_unique<T>(this->componentManager)
      });
    }

    template <class T>
    void addEntity(const Entity e)
    {
      // Maybe add a signature check between the entity and the system?
      this->checkSystemRegistration<T>();

      System& system = this->getSystem<T>();
      system.addEntity(e);
    }

    template <class T>
    void removeEntity(const Entity e)
    {
      this->checkSystemRegistration<T>();

      const std::string systemName = typeid(T).name();

      System& system = this->getSystem<T>();
      if (!system.hasEntity(e)) {
        std::stringstream errorMsgStream;
        errorMsgStream << "Attempted to remove an entity that is not "
                       << "registered in the system, " << systemName << ".";
        const std::string errorMsg = errorMsgStream.str();
        throw UnregisteredEntityError{errorMsg};
      }

      system.removeEntity(e);
    }

    template <class T>
    Signature getSignature() const
    {
      this->checkSystemRegistration<T>();

      System& system = this->getSystem<T>();
      return system.getSignature();
    }

    template <class T>
    T& getSystem() const
    {
      checkSystemRegistration<T>();

      const std::string systemName = typeid(T).name();
      const auto item = this->nameToSystem.find(systemName);
      System& system = *((item->second).get());
      return dynamic_cast<T&>(system);
    }

  private:
    template <class T>
    void checkSystemRegistration() const
    {
      const std::string systemName = typeid(T).name();
      const auto item = this->nameToSystem.find(systemName);
      if (item == this->nameToSystem.end()) {
        std::stringstream errorMsgStream;
        errorMsgStream << "Attempted to use an unregistered system type, "
                       << systemName << ".";
        const std::string errorMsg = errorMsgStream.str();
        throw UnregisteredSystemError{errorMsg};
      }
    }

    ComponentManager& componentManager;

    std::unordered_map<std::string, std::unique_ptr<System>> nameToSystem;
  };
}

#endif
