#include <stdexcept>
#include <string>

#include <catch2/catch.hpp>

#include <planes/engine/ecs/component.hpp>
#include <planes/engine/ecs/entity.hpp>

using namespace planes::engine::ecs;

TEST_CASE("Component Array must be able to manage components properly",
          "[ECS | ComponentArray]")
{
  struct TestComponent
  {
    int x;
  };

  EntityManager<5> entityManager{};
  ComponentArray<TestComponent> components{};

  int numTestEntities = 5;
  Entity entities[numTestEntities];
  for (int i = 0; i < numTestEntities; i++) {
    entities[i] = entityManager.createEntity();
  }

  Entity deletedEntity = entities[numTestEntities - 1];
  entityManager.deleteEntity(deletedEntity);

  SECTION("Adding components to entities should be done properly")
  {
    SECTION("Adds a component to an entity")
    {
      REQUIRE_NOTHROW(([&] {
        components.addComponent(entities[0], TestComponent{});
        components.addComponent(entities[0], TestComponent{});
      })());

      REQUIRE_NOTHROW(components.getComponent(entities[0]));
    }
  }

  SECTION("Getting components of entities should be done properly")
  {
    SECTION("Gets the component of an entity that is within the proper range")
    {
      components.addComponent(entities[0], TestComponent{});

      REQUIRE_NOTHROW(([&] {
        components.getComponent(entities[0]);
      })());
    }

    SECTION("Gets the correct component of an entity that is within the "
            "proper range")
    {
      int testComponentXVals[numTestEntities - 2];
      for (int i = 0; i < numTestEntities - 2; i++) {
        // Remember that the last test entity has been deleted. So, we're not
        // going to create a component for it here.
        TestComponent tc{i};
        components.addComponent(entities[i], tc);

        testComponentXVals[i] = i;
      }

      for (int i = 0; i < numTestEntities - 2; i++) {
        TestComponent* tc = components.getComponent(entities[i]);
        REQUIRE(tc->x == testComponentXVals[i]);
      }
    }

    SECTION("Getting the component of an entity that is within the proper "
            "range should give us a pointer to the component")
    {
      components.addComponent(entities[0], TestComponent{});
      REQUIRE_NOTHROW(([&] {
        TestComponent* tc = components.getComponent(entities[0]);
      })());
    }

    SECTION("Getting the component of an entity that is not within the proper "
            "range should cause an exception")
    {
      // Throwing an exception because of an invalid entity must be handled
      // by the component manager.
      REQUIRE_THROWS_AS(components.getComponent(-1), NoComponentForEntityError);
      REQUIRE_THROWS_AS(components.getComponent(entityManager.kMaxNumEntities),
                        NoComponentForEntityError);
    }

    SECTION("Getting the component of an entity that does not exist should "
            "cause an exception")
    {
      REQUIRE_THROWS_AS(components.getComponent(deletedEntity),
                        NoComponentForEntityError);
    }

    SECTION("Getting a component of an entity who does not have a component "
            "in the component array will cause an exception")
    {
      REQUIRE_THROWS_AS(components.getComponent(entities[0]),
                        NoComponentForEntityError);
    }
  }

  SECTION("Deleting components of entities should be done properly")
  {
    SECTION("Deletes the component of an entity that is within the proper "
            "range")
    {
      components.addComponent(entities[0], TestComponent{});
      components.deleteComponent(entities[0]);

      REQUIRE_THROWS_AS(components.getComponent(entities[0]),
                        NoComponentForEntityError);
    }

    SECTION("Deleting the component of an entity that is not within the proper "
            "range should cause an exception")
    {
      REQUIRE_THROWS_AS(components.deleteComponent(-1),
                        NoComponentForEntityError);
      REQUIRE_THROWS_AS(
        components.deleteComponent(entityManager.kMaxNumEntities),
        NoComponentForEntityError);
    }

    SECTION("Deleting the component of an entity that does not exist should "
            "cause an exception")
    {
      REQUIRE_THROWS_AS(components.deleteComponent(deletedEntity),
                        NoComponentForEntityError);
    }

    SECTION("Deleting a component of an entity who does not have a component "
            "in the component array will cause an exception")
    {
      REQUIRE_THROWS_AS(components.deleteComponent(entities[0]),
                        NoComponentForEntityError);
    }
  }

  SECTION("Notification of a deletion of an entity must be handled properly")
  {
    SECTION("Receiving a notification should invoke deletion of the component "
            "of the deleted entity if and only if the entity has such a "
            "component")
    {
      Entity deletedEntity = entities[0];
      components.addComponent(deletedEntity, TestComponent{});
      entityManager.deleteEntity(deletedEntity);

      components.notifyEntityDeleted(deletedEntity);
      REQUIRE_THROWS_AS(components.getComponent(deletedEntity),
                      NoComponentForEntityError);
    }

    SECTION("Receiving a notification for an entity that does not have a "
            "component held by the component array must not have any effect "
            "to the component array")
    {

      for (int i = 0; i < numTestEntities - 2; i++) {
        // Remember that the last test entity has been deleted. So, we're not
        // going to create a component for it here.
        components.addComponent(entities[i], TestComponent{});
      }

      for (int i = 0; i < numTestEntities - 2; i++) {
        REQUIRE_NOTHROW(([&] {
          components.getComponent(entities[i]);
        })());
      }
    }
  }
}
