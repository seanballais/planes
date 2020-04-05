#include <stdexcept>

#include <planes/engine/ecs/component.hpp>

namespace planes::engine::ecs
{
  NoComponentForEntityError::NoComponentForEntityError(const char* what_arg)
    : std::runtime_error(what_arg) {}

  UnregisteredComponentTypeError::UnregisteredComponentTypeError(
      const char* what_arg)
    : std::runtime_error(what_arg) {}

  ComponentManager::ComponentManager()
    : nextComponentTypeIndex(0) {}

  void ComponentManager::notifyEntityDeleted(const Entity e)
  {
    for (const auto& item : this->typeNameToArrayMap) {
      IComponentArray& componentArray = *(item.second.get());
      componentArray.notifyEntityDeleted(e);
    }
  }
}
