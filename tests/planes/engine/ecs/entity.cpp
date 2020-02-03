// We have to do this little hack so that we can be sure that the behaviour
// of our entity manager is what we expected. This hack is only guaranteed to
// work in Linux/GCC for now. See more information in:
// https://stackoverflow.com/q/6778496/1116098
#define private public

#include <bitset>

#include <catch2/catch.hpp>

#include <planes/engine/ecs/component.hpp>
#include <planes/engine/ecs/entity.hpp>>
#include <planes/utils/assert.hpp>

using namespace planes::engine::ecs;
using namespace planes::utils;

// Some test cases will only run when compiling in debug mode. This is because
// assertion will only be triggered in debug mode.

TEST_CASE("Entity Manager must be able to manage entities properly",
		      "[ECS | Entity]")
{
  EntityManager entityManager();

  SECTION("Creating entities should be done properly")
  {
    SECTION("Creates an entity whose ID is within the proper range")
    {
      Entity entity = entityManager.createEntity();

      REQUIRE(entity >= 0 && entity < MAX_NUM_ENTITIES);
    }
  }

  SECTION("Deleting entities should be done properly")
  {
    #if DEBUG
    SECTION("Deleting entity IDs not within the proper range"
            + "will cause an assertion error")
    {
      // Passing an ID of MAX_NUM_ENTITIES should raise an error since
      // the entity IDs start at 0.
      REQUIRE_THROWS_AS(entityManager.deleteEntity(MAX_NUM_ENTITIES),
                        AssertionError)
    }
    #endif

    SECTION("Deleted entity's signature must be cleared")
    {
      Entity entity = entityManager.createEntity();
      entityManager.deleteEntity(entity);

      std::bitset<MAX_NUM_COMPONENTS> emptySignature;
      REQUIRE(entityManager.entitySignatures[entity] == emptySignature)
    }
  }
}
