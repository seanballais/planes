#ifndef PLANES_ENGINE_ECS_COMPONENT_HPP_
#define PLANES_ENGINE_ECS_COMPONENT_HPP_

#include <memory>
#include <sstream>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include <planes/engine/ecs/entity.hpp>

namespace planes::engine::ecs {
  class NoComponentForEntityError : std::runtime_error
  {
  public:
    NoComponentForEntityError(const char* what_arg);
  };

  class IComponentArray
  {
  public:
    virtual ~IComponentArray() = default;
    virtual void notifyEntityDeleted(Entity entity) = 0;
  };

  template <class T>
  class ComponentArray : public IComponentArray
  {
  public:
    ComponentArray() {}

    void addComponent(const Entity entity, const T component)
    {
      const int entityIndex = this->components.size();
      this->components.push_back(component);
      this->entityToComponentMap.insert({entity, entityIndex});
    }

    T* getComponent(const Entity entity)
    {
      auto item = this->getComponentFromEntity(entity);
      const int entityIndex = item->second;
      return &(this->components[entityIndex]);
    }

    void deleteComponent(const Entity entity)
    {
      auto item = this->getComponentFromEntity(entity);
      const int deletedEntityIndex = item->second;
      this->entityToComponentMap.erase(item);
      T lastComponent = this->components[components.size() - 1];
      this->components[deletedEntityIndex] = lastComponent;
      this->components.pop_back();
    }

    void notifyEntityDeleted(const Entity entity) override
    {
      try {
        auto item = this->getComponentFromEntity(entity);
      } catch (NoComponentForEntityError e) {
        return;
      }

      this->deleteComponent(entity);
    }

  private:
    std::unordered_map<Entity, uint32_t>::iterator
    getComponentFromEntity(const Entity entity)
    {
      auto item = this->entityToComponentMap.find(entity);
      if (item == this->entityToComponentMap.end()) {
        std::stringstream errorMsgStream;
        errorMsgStream << "Entity" << entity
                       << " does not have a component we hold.";
        const std::string errorMsg = errorMsgStream.str();
        throw NoComponentForEntityError(errorMsg.c_str());
      }

      return item;
    }

    std::vector<T> components;
    std::unordered_map<Entity, uint32_t> entityToComponentMap;
  };

  class UnregisteredComponentTypeError : public std::runtime_error
  {
  public:
    UnregisteredComponentTypeError(const char* what_arg);
  };

  class ComponentManager
  {
  public:
    ComponentManager()
      : nextComponentTypeIndex(0) {}

    template <typename T>
    void registerComponentType()
    {
      // There should be an error when a component type has been registered
      // twice.
      const std::string typeName = typeid(T).name();
      this->typeNameToArrayMap.insert(
        typeName,
        std::make_unique<ComponentArray<T>>());
      this->typeNameToIndexMap.insert({typeName, this->nextComponentTypeIndex});

      this->nextComponentTypeIndex++;
    }

    template <typename T>
    unsigned int getComponentTypeIndex()
    {
      // This should really only be done in debug mode.
      this->checkComponentTypeRegistration<T>();

      const std::string typeName = typeid(T).name();
      return this->typeNameToIndexMap[typeName];
    }

    template <typename T>
    T* getComponent(const Entity e)
    {
      // This should really only be done in debug mode.
      this->checkComponentTypeRegistration<T>();

      return &(this->getComponentTypeArray<T>()
                    .getComponent(e));
    }

    template <typename T>
    void addComponentType(const Entity e)
    {
      // This should really only be done in debug mode.
      this->checkComponentTypeRegistration<T>();

      this->getComponentTypeArray<T>()
           .addComponent(e, T{});
    }

    template <typename T>
    void deleteComponentType(const Entity e)
    {
      // This should really only be done in debug mode.
      this->checkComponentTypeRegistration<T>();

      this->getComponentTypeArray<T>()
           .deleteComponent(e);
    }

    void notifyEntityDeleted(const Entity e);

  private:
    template <typename T>
    void checkComponentTypeRegistration() const
    {
      const std::string typeName = typeid(T).name();
      const auto item = this->typeNameToArrayMap.find(typeName);
      if (item == this->typeNameToArrayMap.end()) {
        std::stringstream errorMsgStream;
        errorMsgStream << "Attempted to get the index of component type, "
                       << typeName << ".";
        const std::string errorMsg = errorMsgStream.str();
        throw UnregisteredComponentTypeError(errorMsg.c_str());
      }
    }

    template <typename T>
    ComponentArray<T>& getComponentTypeArray()
    {
      const std::string typeName = typeid(T).name();
      return *(this->typeNameToArrayMap[typeName]);
    }

    std::unordered_map<std::string, std::unique_ptr<IComponentArray>>
      typeNameToArrayMap;
    std::unordered_map<std::string, unsigned int> typeNameToIndexMap;

    unsigned int nextComponentTypeIndex;
  };
}

#endif
