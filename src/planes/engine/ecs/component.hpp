#ifndef PLANES_ENGINE_ECS_COMPONENT_HPP_
#define PLANES_ENGINE_ECS_COMPONENT_HPP_

#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include <planes/engine/ecs/entity.hpp>

namespace planes::engine::ecs {
  class NoComponentForEntityError;

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

    void addComponent(Entity entity, T component)
    {
      const int entityIndex = this->components.size();
      this->components.push_back(component);
      this->entityToComponentMap.insert({entity, entityIndex});
    }

    T* getComponent(Entity entity)
    {
      auto item = this->getComponentFromEntity(entity);
      const int entityIndex = item->second;
      return &(this->components[entityIndex]);
    }

    void deleteComponent(Entity entity)
    {
      auto item = this->getComponentFromEntity(entity);
      const int deletedEntityIndex = item->second;
      this->entityToComponentMap.erase(item);
      T lastComponent = this->components[components.size() - 1];
      this->components[deletedEntityIndex] = lastComponent;
      this->components.pop_back();
    }

    void notifyEntityDeleted(Entity entity) override
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
    getComponentFromEntity(Entity entity)
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

  class NoComponentForEntityError : std::runtime_error
  {
  public:
    NoComponentForEntityError(const char* what_arg);
  };
}

#endif
