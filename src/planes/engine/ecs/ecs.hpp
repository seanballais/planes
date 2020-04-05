#ifndef PLANES_ENGINE_ECS_ECS_HPP_
#define PLANES_ENGINE_ECS_ECS_HPP_

#include <sstream>
#include <typeinfo>

#include <planes/engine/ecs/component.hpp>
#include <planes/engine/ecs/entity.hpp>
#include <planes/engine/ecs/system.hpp>

namespace planes::engine::ecs
{
  class ECS
  {
  public:
    ECS();

    Entity createEntity();
    void deleteEntity(const Entity e);

    template <class T>
    void registerComponentType()
    {
      // TODO: There should be an error if a component type has been registered
      //       twice. Maybe implement this at the ComponentManager level.
      this->componentManager.registerComponentType<T>();
    }

    template <class T>
    void addComponentTypeToEntity(const Entity e)
    {
      this->checkIfEntityExists(e);
      
      // We need to change the signature of an entity every time a component
      // type has been added to it.
      this->componentManager.addComponentTypeToEntity<T>(e);
      this->addComponentTypeToEntitySignature<T>(e);
    }

    template <class T>
    void removeComponentTypeFromEntity(const Entity e)
    {
      this->checkIfEntityExists(e);

      // We need to change the signature of an entity every time a component
      // type has been removed from it.
      this->componentManager.removeComponentTypeFromEntity<T>(e);
      this->removeComponentTypeFromEntitySignature<T>(e);
    }

    template <class T>
    T& getEntityComponentType(const Entity e)
    {
      this->checkIfEntityExists(e);

      return this->componentManager.getEntityComponentType<T>(e);
    }

    template <class T>
    void registerSystem()
    {
      this->systemManager.registerSystem<T>();
    }

    template <class T>
    T& getSystem()
    {
      return this->systemManager.getSystem<T>();
    }

    template <class T>
    Signature getSystemSignature()
    {
      return this->systemManager.getSystemSignature<T>();
    }

    template <class T>
    void addEntityToSystem(const Entity e)
    {
      this->checkIfEntityExists(e);

      Signature entitySignature = this->entityManager.getEntitySignature(e);
      Signature systemSignature = this->systemManager.getSystemSignature<T>();
      if ((entitySignature & systemSignature) == systemSignature) {
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
        this->systemManager.addEntityToSystem<T>(e);
      } else {
        const std::string systemName = typeid(T).name();
        std::stringstream errorMsgStream;
        errorMsgStream << "Signature of entity, " << e << ", does not match"
                       << "that of the system, " << systemName << ".";
        const std::string errorMsg = errorMsgStream.str();
        throw IncompatibleEntitySignatureError{errorMsg};
      }
    }

    template <class T>
    void removeEntityFromSystem(const Entity e)
    {
      this->checkIfEntityExists(e);

      this->systemManager.removeEntityFromSystem<T>(e);
    }

  private:
    void checkIfEntityExists(const Entity e);

    template <class T>
    void addComponentTypeToEntitySignature(const Entity e)
    {
      this->updateComponentTypeStatusInEntitySignature<T>(e, true);
    }

    template <class T>
    void removeComponentTypeFromEntitySignature(const Entity e)
    {
      this->updateComponentTypeStatusInEntitySignature<T>(e, false);
    }

    template <class T>
    void updateComponentTypeStatusInEntitySignature(const Entity e,
                                                    const bool isTypeAdded)
    {
      const unsigned int typeIndex = this->componentManager
                                          .getComponentTypeIndex<T>();
      Signature signature = this->entityManager.getEntitySignature(e);
      signature.set(static_cast<size_t>(typeIndex), isTypeAdded);
      this->entityManager.setEntitySignature(e, signature);

      this->systemManager.notifyEntitySignatureChanged(e, signature);
    }

    EntityManager<1000> entityManager;
    ComponentManager componentManager;
    SystemManager systemManager;
  };
}

#endif
