#include <stdexcept>

#include <catch2/catch.hpp>

#include <planes/engine/ecs/component.hpp>
#include <planes/engine/ecs/entity.hpp>
#include <planes/engine/ecs/system.hpp>

using namespace planes::engine::ecs;

TEST_CASE("System must be able to perform its tasks properly "
          "[ECS | System]")
{
  struct TestComponent
  {
    int x;
  };

  ComponentManager componentManager{};
  componentManager.registerComponentType<TestComponent>();

  class TestSystem : public System
  {
  public:
    TestSystem(ComponentManager& componentManager)
      : System(Signature{1}, componentManager) {}

    void update() override
    {
      for (const Entity e : this->entities) {
        TestComponent& c = this->componentManager
                                .getComponent<TestComponent>(e);
        if (c.x == 1) {
          c.x = 0;
        }
      }
    }
  };

  TestSystem system{componentManager};

  const unsigned int numTestEntities = 3;
  EntityManager<numTestEntities> entityManager{};
  Entity entities[numTestEntities];
  for (int i = 0; i < numTestEntities; i++) {
    entities[i] = entityManager.createEntity();
    componentManager.addComponentType<TestComponent>(entities[i]);
  }

  Entity deletedEntity = entities[numTestEntities - 1];
  entityManager.deleteEntity(deletedEntity);
  componentManager.notifyEntityDeleted(deletedEntity);

  SECTION("Adding entities to a system should be done properly")
  {
    SECTION("Adds an entity with the proper range")
    {
      const Entity e = entities[0];
      TestComponent& c = componentManager.getComponent<TestComponent>(e);
      c.x = 1;

      REQUIRE_NOTHROW(system.addEntity(e));

      system.update();

      REQUIRE(c.x == 0);
    }

    SECTION("Adds an entity that is not within the proper range")
    {
      // The system doesn't really care whether or not an entity is in the
      // proper range.
      REQUIRE_NOTHROW(system.addEntity(-1));
      REQUIRE_NOTHROW(system.addEntity(entityManager.kMaxNumEntities));
    }

    SECTION("Adds an entity that has been already deleted")
    {
      // The system wouldn't know that the entity has been deleted already, so
      // it should still add it.
      REQUIRE_NOTHROW(system.addEntity(deletedEntity));
    }

    SECTION("Adds two entities within the proper range")
    {
      const unsigned int numTestEntities = 2;
      for (int i = 0; i < numTestEntities; i++) {
        const Entity e = entities[i];

        TestComponent& c = componentManager.getComponent<TestComponent>(e);
        c.x = 1;

        system.addEntity(e);
      }

      system.update();

      for (int i = 0; i < numTestEntities; i++) {
        const Entity e = entities[i];
        TestComponent& c = componentManager.getComponent<TestComponent>(e);
        REQUIRE(c.x == 0);
      }
    }

    SECTION("Adding the same entity twice should cause an exception")
    {
      const Entity e = entities[0];
      system.addEntity(e);
      REQUIRE_THROWS_AS(system.addEntity(e), EntityAlreadyExistsError);
    }
  }

  SECTION("Removing entities should be done properly")
  {
    SECTION("Removes an entity that has been added")
    {
      const Entity e = entities[0];
      TestComponent& c = componentManager.getComponent<TestComponent>(e);
      c.x = 1;

      system.addEntity(e);

      REQUIRE_NOTHROW(system.removeEntity(e));

      system.update();

      REQUIRE(c.x != 0);
    }

    SECTION("Removing an entity that has not been added should cause an "
            "exception")
    {
      const Entity e = entities[0];
      REQUIRE_THROWS_AS(system.removeEntity(e), UnregisteredEntityError);
    }

    SECTION("Removing an entity that was already removed should cause an "
            "exception")
    {
      const Entity e = entities[0];
      system.addEntity(e);
      system.removeEntity(e);

      REQUIRE_THROWS_AS(system.removeEntity(e), UnregisteredEntityError);
    }
  }

  SECTION("System updates should be done properly")
  {
    SECTION("Updates a system properly with all entities existing")
    {
      const unsigned int numTestEntities = 2;
      for (int i = 0; i < numTestEntities; i++) {
        const Entity e = entities[i];
  
        TestComponent& c = componentManager.getComponent<TestComponent>(e);
        c.x = 1;

        system.addEntity(e);
      }

      REQUIRE_NOTHROW(system.update());

      for (int i = 0; i < numTestEntities; i++) {
        const Entity e = entities[i];
        TestComponent& c = componentManager.getComponent<TestComponent>(e);
        REQUIRE(c.x == 0);
      }
    }

    SECTION("Updating a system that has at least one non-existent entity "
            "should cause an exception")
    {
      for (int i = 0; i < numTestEntities; i++) {
        const Entity e = entities[i];
        system.addEntity(e);
      }

      REQUIRE_THROWS(system.update());
    }
  }
}
