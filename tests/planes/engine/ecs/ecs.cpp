#include <catch2/catch.hpp>

#include <planes/engine/ecs/component.hpp>
#include <planes/engine/ecs/entity.hpp>
#include <planes/engine/ecs/system.hpp>

using namespace planes::engine::ecs;

// TODO: Test the appropriate functions with two entities.

TEST_CASE("ECS must be able to perform its tasks properly "
          "[ECS]")
{
  struct TestComponent
  {
    int x;
  };

  class TestSystem : public System
  {
  public:
    TestSystem(ComponentManager& componentManager)
      : System(Signature{1}, componentManager) {}

    void update() overrride
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

  ECS ecs{};

  SECTION("Creating an entity must be done properly")
  {
    REQUIRE_NOTHROW(ecs.createEntity());
  }

  SECTION("Deleting an entity must be done properly")
  {
    SECTION("Deletes an existing entity")
    {
      const Entity e = ecs.createEntity();
      REQUIRE_NOTHROW(ecs.deleteEntity(e));

      ecs.registerComponentType<TestComponent>();

      REQUIRE_THROWS_AS(ecs.addComponentToEntity<TestComponent>(e),
                        NonExistentEntityError);
    }

    SECTION("Deleting a non-existent entit should cause an exception")
    {
      const Entity nonExistentEntity = 15;
      REQUIRE_THROWS_AS(ecs.deleteEntity(nonExistentEntity),
                        NonExistentEntityError);
    }
  }

  SECTION("Registering components must be done properly")
  {
    REQUIRE_NOTHROW(ecs.registerComponentType<TestComponent>());

    const Entity e = ecs.createEntity();
    REQUIRE_NOTHROW(ecs.addComponentToEntity<TestComponent>(e));

    // TODO: There should be an error when a component type has been registered
    //       twice.
  }

  SECTION("Adding components to entities must be done properly")
  {
    ecs.registerComponentType<TestComponent>();

    SECTION("Adds a component to an existing entity")
    {
      const Entity e = ecs.createEntity();
      REQUIRE_NOTHROW(ecs.addComponentToEntity<TestComponent>(e))
      REQUIRE_NOTHROW(ecs.removeComponentFromEntity<TestComponent>(e));
    }

    SECTION("Adding a component to a non-existent entity should cause an "
            "exception")
    {
      const Entity e = ecs.createEntity();
      REQUIRE_THROWS_AS(ecs.addComponentToEntity<TestComponent>(e),
                        NonExistentEntityError)
    }

    // NOTE: No need to test with a non-existent component type since doing so
    //       will result in a compilation error.
  }

  SECTION("Removing components from entities must be done properly")
  {
    ecs.registerComponentType<TestComponent>();

    SECTION("Removes a component from an existing entity")
    {
      const Entity e = ecs.createEntity();
      ecs.addComponentToEntity<TestComponent>(e);

      REQUIRE_NOTHROW(ecs.removeComponentFromEntity<TestComponent>(e));

      // Just to check that we really have removed the component from the
      // entity.
      REQUIRE_NOTHROW(ecs.addComponentToEntity<TestComponent>(e));
    }

    SECTION("Removing a component from a non-existent entity should cause an "
            "exception")
    {
      const Entity nonExistentEntity = 15;
      REQUIRE_THROWS_AS(ecs.removeComponentFromEntity<TestComponent>(e),
                        NonExistentEntityError);
    }

    SECTION("Removing a component from an entity that does not have such a "
            "component should cause an exception")
    {
      const Entity e = ecs.createEntity();
      REQUIRE_THROWS_AS(ecs.removeComponentFromEntity<TestComponent>(e),
                        NoComponentForEntityError);
    }
  }

  SECTION("Getting a component type's index must be done properly")
  {
    const unsigned int componentIndex = 1;
    ecs.registerComponentType<TestComponent>();

    REQUIRE(ecs.getComponentTypeIndex<TestComponent>() == componentIndex);

    // NOTE: No need to test for registering a non-existent component type since
    //       it will result in compilation error.
  }

  SECTION("Getting a component type of an entity must be done properly")
  {
    ecs.registerComponentType<TestComponent>;

    SECTION("Gets the component type of an existing entity")
    {
      const Entity e = ecs.createEntity();
      ecs.addComponentToEntity<TestComponent>(e);

      REQUIRE_NOTHROW(([&] () {
        TestComponent& c = ecs.getComponentTypeOfEntity<TestComponent>(e);
      }));
    }

    SECTION("Getting the component type of a non-existent entity should cause "
            "an exception")
    {
      const Entity nonExistentEntity = 15;
      REQUIRE_THROWS_AS(
        ecs.getComponentTypeOfEntity<TestComponent>(nonExistentEntity),
        NonExistentEntityError);
    }
  }

  SECTION("Registering systems must be done properly")
  {
    ecs.registerSystem<TestSystem>();
    REQUIRE_NOTHROW(ecs.getSystem<TestSystem>());

    // TODO: There should be an error when a system has been registered twice.
  }

  SECTION("Getting a system must be done properly")
  {
    ecs.registerSystem<TestSystem>();
    REQUIRE_NOTHROW(([&] () {
      System& system = ecs.getSystem<TestSystem>();
    }));
  }

  SECTION("Adding an entity to a system must be done properly")
  {
    const Entity e = ecs.createEntity();
    ecs.registerComponentType<TestComponent>()
    ecs.addComponentToEntity<TestComponent>(e);

    TestComponent& c = ecs.getComponentTypeOfEntity<TestComponent>(e);
    c.x = 1;

    ecs.registerSystem<TestSystem>()

    SECTION("Adds an existing entity to a system")
    {
      REQUIRE_NOTHROW(ecs.addEntityToSystem<TestSystem>(e));

      System& system = ecs.getSystem<TestSystem>();
      system.update();

      REQUIRE(c.x == 0);
    }

    SECTION("Adding a non-existent entity to a system should cause an "
            "exception")
    {
      REQUIRE_THROWS_AS(ecs.addEntityToSystem<TestSystem>(e),
                        NonExistentEntityError);
    }

    SECTION("Adding the same entity twice should cause an exception")
    {
      REQUIRE_NOTHROW(ecs.addEntityToSystem<TestSystem>(e));
      REQUIRE_THROWS_AS(ecs.addEntityToSystem<TestSystem>(e),
                        EntityAlreadyAddedToSystemError);
    }

    SECTION("Adding an entity whose signature does not match the system's "
            "should cause an exception")
    {
      const Entity e = ecs.createEntity();
      REQUIRE_THROWS_AS(ecs.addEntityToSystem<TestSystem>(e),
                        IncompatibleEntitySignatureError);
    }
  }
}
