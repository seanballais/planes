/** Incomplete and must be excluded from being compiled for now until we
    are finished with implementing the custom assertion function.
#include <catch2/catch.hpp>

#include <planes/engine/ecs/entity.hpp>

using namespace planes::engine::ecs;

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
    SECTION("IDs of entities to be deleted should be within the proper range")
    {
      
    }
  }
}
*/