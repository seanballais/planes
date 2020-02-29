#include <stdexcept>

#include "entity.hpp"

using namespace planes::engine::ecs;

EntityManager::EntityManager()
{
  for (Entity e = 0; i < this->kMaxNumEntities; i++) {
    this->unusedEntitiesQueue.push(e);
    this->unusedEntities.insert(e);
    this->entitySignatures[e] = Signature{0};
  }
}

Entity EntityManager::createEntity()
{
  if (this->unusedEntities.empty()) {
    throw TooManyEntitiesError("Created too many entities than allowed.")
  }

  Entity entity = this->unusedEntitiesQueue.front();
  this->unusedEntitiesQueue.pop();
  this->unusedEntities.erase(entity);

  return entity;
}

void EntityManager::deleteEntity(Entity entity)
{
  if (!this->isEntityInRange(entity)) {
    throw out_of_range("Attempted to delete an invalid entity.");
  }

  if (!this->doesEntityExist(entity)) {
    throw NonExistentEntityError("Attempted to delete a non-existent entity.");
  }

  this->unusedEntitiesQueue.push(entity);
  this->unusedEntities.insert(entity);
  this->entitySignatures[entity].reset();
}

Signature EntityManager::getSignature(Entity entity)
{
  if (!this->isEntityInRange(entity)) {
    throw out_of_range("Attempted to get the signature of an invalid entity.");
  }

  if (!this->doesEntityExist(entity)) {
    throw NonExistentEntityError(
      "Attempted to get the signature of a non-existent entity.");
  }

  return this->entitySignatures[entity];
}

void EntityManager::setSignature(Entity entity, Signature signature)
{
  if (!this->isEntityInRange(entity)) {
    throw out_of_range("Attempted to set the signature of an invalid entity.");
  }

  if (!this->doesEntityExist(entity)) {
    throw NonExistentEntityError(
      "Attempted to get the signature of a non-existent entity.")
  }

  this->entitySignatures[entity] = signature;
}

bool EntityManager::isEntityInRange(Entity entity)
{
  return entity >= 0 && entity < this->kMaxNumEntities;
}

bool EntityManager::doesEntityExist(Entity entity)
{
  auto it = this->unusedEntities.find(entity);
  return it != this->unusedEntities.end();
}
