#include "entity.hpp"

using namespace planes::engine::ecs;

EntityManager::EntityManager()
{
  for (int i = 0; i < this->MAX_NUM_ENTITIES; i++) {
    this->unusedEntities.push(i);
  }
}

Entity EntityManager::createEntity()
{
  if (this->unusedEntities.empty()) {
    throw TooManyEntitiesError("Created too many entities than allowed.")
  }

  Entity entity = this->unusedEntities.front();
  this->unusedEntities.pop();

  return entity;
}
