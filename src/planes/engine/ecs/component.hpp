#ifndef PLANES_ENGINE_ECS_COMPONENT_HPP_
#define PLANES_ENGINE_ECS_COMPONENT_HPP_

#include <cstdint>

namespace planes::engine::ecs {
  using ComponentType = std::uint8_t;

  const std::uint8_t MAX_NUM_COMPONENTS = 64;
}

#endif
