#include <stdexcept>
#include <string>

#include <planes/engine/ecs/entity.hpp>

namespace planes::engine::ecs
{
  NonExistentEntityError::NonExistentEntityError(const char* what_arg)
    : std::runtime_error(what_arg) {}

  NonExistentEntityError::NonExistentEntityError(const std::string what_arg)
    : std::runtime_error(what_arg) {}

  TooManyEntitiesError::TooManyEntitiesError(const char* what_arg)
	  : std::runtime_error(what_arg) {}
}
