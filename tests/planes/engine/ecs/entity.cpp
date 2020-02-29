#include <stdexcept>
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

      REQUIRE((entity >= 0 && entity < entityManager.kMaxNumEntities));
    }

    SECTION("Creating more entities than the maximum should cause an exception")
    {
      for (int i = 0; i < entityManager.kMaxNumEntities; i++) {
        entityManager.createEntity();
      }

      REQUIRE_THROWS_AS(entityManager.createEntity(), TooManyEntitiesError);
    }

    SECTION("Creates an entity with an empty signature")
    {
      Entity entity = entityManager.createEntity();

      REQUIRE(entityManager.getSignature(entity) == 0);
    }
  }

  SECTION("Deleting entities should be done properly")
  {
    SECTION(std::string("Deleting entity IDs not within the proper range ")
            + std::string("will cause an out of range exception"))
    {
      // Passing an ID of kMaxNumEntities should raise an error since
      // the entity IDs start at 0.
      REQUIRE_THROWS_AS(
        entityManager.deleteEntity(entityManager.kMaxNumEntities),
        std::out_of_range);
    }

    SECTION("Deleting an already deleted entity should cause an exception")
    {
      Entity entity = entityManager.createEntity();
      entityManager.deleteEntity(entity);

      REQUIRE_THROWS_AS(entityManager.deleteEntity(entity),
                        NonExistentEntityError);
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

    SECTION("Setting a signature to an invalid entity will cause an exception")
    {
      Signature signature{1};
      REQUIRE_THROWS_AS(
        entityManager.setSignature(entityManager.kMaxNumEntities, signature),
        std::out_of_range);
    }

    SECTION(std::string("Setting a signature to a non-existent ")
            + std::string("entity will cause an exception"))
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
      Signature signature{1};
      entityManager.setSignature(entity, signature);

      REQUIRE(entityManager.getSignature(entity) == signature);
    }

    SECTION(std::string("Getting the signature of an invalid entity ")
            + std::string("will cause an out of range exception"))
    {
      // Passing an ID of kMaxNumEntities should raise an error since
      // the entity IDs start at 0.
      REQUIRE_THROWS_AS(
        entityManager.getSignature(entityManager.kMaxNumEntities),
        std::out_of_range);
    }

    SECTION(std::string("Getting a signature of a non-existent ")
            + std::string("entity will cause an exception"))
    {
      Entity entity = entityManager.createEntity();

      // To make sure we use a non-existent entity.
      entityManager.deleteEntity(entity);

      REQUIRE_THROWS_AS(entityManager.getSignature(entity),
                        NonExistentEntityError);
    }
  }
}
