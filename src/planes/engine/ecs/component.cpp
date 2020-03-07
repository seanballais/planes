#include <stdexcept>

#include <planes/engine/ecs/component.hpp>

namespace planes::engine::ecs
{
  NoComponentForEntityError::NoComponentForEntityError(const char* what_arg)
    : std::runtime_error(what_arg) {}
}
