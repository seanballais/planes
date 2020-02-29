#include <stdexcept>

#include <planes/engine/ecs/entity.hpp>

using namespace planes::engine::ecs;

TooManyEntitiesError::TooManyEntitiesError(const char* what_arg)
  : std::runtime_error(what_arg) {}

NonExistentEntityError::NonExistentEntityError(const char* what_arg)
  : std::runtime_error(what_arg) {}
