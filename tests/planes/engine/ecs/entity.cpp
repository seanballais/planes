#include <string>

#include <catch2/catch.hpp>

#include <planes/engine/ecs/component.hpp>
#include <planes/engine/ecs/entity.hpp>
#include <planes/utils/assert.hpp>

using namespace planes::engine::ecs;
using namespace planes::utils;

// Some test cases will only run when compiling in debug mode. This is because
// assertion will only be triggered in debug mode.

TEST_CASE("Entity Manager must be able to manage entities properly",
		      "[ECS | Entity]")
{
  EntityManager<5> entityManager{};

  SECTION("Creating entities should be done properly")
  {
    SECTION("Creates an entity whose ID is within the proper range")
    {
      Entity entity = entityManager.createEntity();

      REQUIRE(entity >= 0 && entity < entityManager.kMaxNumEntities);
    }

    SECTION("Creating more entities than the maximum should cause an exception")
    {
      for (int i = 0; i < entityManager.kMaxNumEntities; i++) {
        entityManager.createEntity();
      }

      REQUIRE_THROWS_AS(entityManager.createEntity(), TooManyEntitiesError);
    }
  }

  SECTION("Deleting entities should be done properly")
  {
    SECTION(std::string("Deleting entity IDs not within the proper range ")
            + std::string("will cause an exception"))
    {
      // Passing an ID of kMaxNumEntities should raise an error since
      // the entity IDs start at 0.
      REQUIRE_THROWS_AS(
        entityManager.deleteEntity(entityManager.kMaxNumEntities),
        OutOfRangeError);
    }

    SECTION("Deleted entity's signature must be cleared on deletion")
    {
      Entity entity = entityManager.createEntity();
      entityManager.deleteEntity(entity);

      // Using .getSignature() here will result in an assertion error.
      Signature emptySignature;
      REQUIRE(entityManager.entitySignatures[entity] == emptySignature)
    }
  }

  SECTION("Setting the signature of entities should be done properly")
  {
    SECTION("Entity must have its signature set correctly")
    {
      Entity entity = entityManager.createEntity();
      
      Signature randomSignature;
      randomSignature[0] = 1;
      entityManager.setSignature(entity, randomSignature);

      REQUIRE(entityManager.getSignature(entity) == randomSignature);
    }

    SECTION("Setting a signature to a non-existent entity will cause an "
            + "exception")
    {
      Entity entity = entityManager.createEntity();

      // To make sure we use a non-existent entity.
      entityManager.deleteEntity(entity);

      Signature emptySignature;
      REQUIRE_THROWS_AS(entityManager.setSignature(entity, emptySignature),
                        NonExistentEntityError);
    }
  }

  SECTION("Getting the signature of entities should be done properly")
  {
    SECTION("Getting the entity signature must give entity's actual signature")
    {
      Entity entity = entityManager.createEntity();
      
      Signature randomSignature;
      randomSignature[0] = 1;

      REQUIRE(entityManager.getSignature(entity) == randomSignature);
    }

    SECTION("Getting a signature of a non-existent entity will cause an "
            + "assertion error")
    {
      Entity entity = entityManager.createEntity();

      // To make sure we use a non-existent entity.
      entityManager.deleteEntity(entity);

      REQUIRE_THROWS_AS(entityManager.getSignature(entity),
                        NonExistentEntityError);
    }
  }
}
