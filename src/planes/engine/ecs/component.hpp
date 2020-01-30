#ifndef PLANES_ENGINE_ECS_COMPONENT_HPP
#define PLANES_ENGINE_ECS_COMPONENT_HPP

#include <cstdint>

namespace planes::engine::ecs {
  using ComponentType = std::uint8_t;

  const std::uint8_t MAX_NUM_COMPONENTS = 64;
}

#endif
