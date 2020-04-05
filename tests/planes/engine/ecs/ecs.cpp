#include <catch2/catch.hpp>

#include <planes/engine/ecs/component.hpp>
#include <planes/engine/ecs/ecs.hpp>
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
        : System(componentManager)
    {
      this->registerRequiredComponentType<TestComponent>();
    }

    void update() override
    {
      for (const Entity e : this->entities) {
        TestComponent& c = this->componentManager
                                .getEntityComponentType<TestComponent>(e);
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

      REQUIRE_THROWS_AS(ecs.addComponentTypeToEntity<TestComponent>(e),
                        NonExistentEntityError);
    }

    SECTION("Deleting a non-existent entity should cause an exception")
    {
      // This test section is pretty much the same as a testing for deleting
      // an entity that was previously created and then already deleted.
      const Entity nonExistentEntity = 15;
      REQUIRE_THROWS_AS(ecs.deleteEntity(nonExistentEntity),
                        NonExistentEntityError);
    }
  }

  SECTION("Registering components must be done properly")
  {
    REQUIRE_NOTHROW(ecs.registerComponentType<TestComponent>());

    const Entity e = ecs.createEntity();
    REQUIRE_NOTHROW(ecs.addComponentTypeToEntity<TestComponent>(e));

    // TODO: There should be an error when a component type has been registered
    //       twice.
  }

  SECTION("Adding components to entities must be done properly")
  {
    ecs.registerComponentType<TestComponent>();

    SECTION("Adds a component to an existing entity")
    {
      const Entity e = ecs.createEntity();
      REQUIRE_NOTHROW(ecs.addComponentTypeToEntity<TestComponent>(e));
      REQUIRE_NOTHROW(ecs.removeComponentTypeFromEntity<TestComponent>(e));
    }

    SECTION("Adding a component to a non-existent entity should cause an "
            "exception")
    {
      const Entity nonExistentEntity = 15;
      REQUIRE_THROWS_AS(
        ecs.addComponentTypeToEntity<TestComponent>(nonExistentEntity),
        NonExistentEntityError);
    }

    SECTION("Adding a component type to an entity should change the entity's "
            "signature")
    {
      ecs.registerSystem<TestSystem>();

      const Entity e = ecs.createEntity();
      REQUIRE_THROWS(ecs.addEntityToSystem<TestSystem>(e));

      // Entity signature changes here.
      ecs.addComponentTypeToEntity<TestComponent>(e);

      // Entity should already have been automatically added to the appropriate
      // systems.
      REQUIRE_THROWS(ecs.addEntityToSystem<TestSystem>(e));
    }

    SECTION("Adding a component type to an entity should invoke the ECS module "
            "to notify the systems the entity is in about the change and move "
            "the entity to the appropriate systems.")
    {
      struct TestComponent0
      {
        int y;
      };

      class TestSystem0 : public System
      {
      public:
        TestSystem0(ComponentManager& componentManager)
            : System(componentManager)
        {
          this->registerRequiredComponentType<TestComponent>();
          this->registerRequiredComponentType<TestComponent0>();
        }

        void update() override {}
      };

      ecs.registerComponentType<TestComponent>();
      ecs.registerComponentType<TestComponent0>();

      ecs.registerSystem<TestSystem>();
      ecs.registerSystem<TestSystem0>();

      const Entity e = ecs.createEntity();
      REQUIRE_NOTHROW(ecs.addComponentTypeToEntity<TestComponent>(e));
      REQUIRE_NOTHROW(ecs.addComponentTypeToEntity<TestComponent0>(e));

      // Since the entity signature and TestSystem's signature no longer match.
      REQUIRE_THROWS(ecs.addEntityToSystem<TestSystem>(e));

      // Since the entity has already been added to TestSystem0.
      REQUIRE_THROWS(ecs.addEntityToSystem<TestSystem0>(e));
    }

    // NOTE: No need to test with a non-existent component type since doing so
    //       will result in a compilation error.
  }

  SECTION("Removing components from entities must be done properly")
  {
    ecs.registerComponentType<TestComponent>();

    SECTION("Removes a component type from an existing entity")
    {
      const Entity e = ecs.createEntity();
      ecs.addComponentTypeToEntity<TestComponent>(e);

      REQUIRE_NOTHROW(ecs.removeComponentTypeFromEntity<TestComponent>(e));

      // Just to check that we really have removed the component from the
      // entity.
      REQUIRE_NOTHROW(ecs.addComponentTypeToEntity<TestComponent>(e));
    }

    SECTION("Removing a component type from a non-existent entity should cause "
            "an exception")
    {
      const Entity nonExistentEntity = 15;
      REQUIRE_THROWS_AS(
        ecs.removeComponentTypeFromEntity<TestComponent>(nonExistentEntity),
        NonExistentEntityError);
    }

    SECTION("Removing a component type from an entity that does not have such "
            "a component should cause an exception")
    {
      const Entity e = ecs.createEntity();
      REQUIRE_THROWS_AS(ecs.removeComponentTypeFromEntity<TestComponent>(e),
                        NoComponentForEntityError);
    }

    SECTION("Removing a component type from an entity should change the "
            "entity's signature")
    {
      ecs.registerSystem<TestSystem>();

      const Entity e = ecs.createEntity();
      ecs.addComponentTypeToEntity<TestComponent>(e);

      // The entity should already have a TestComponent at this point.
      REQUIRE_THROWS(ecs.addEntityToSystem<TestSystem>(e));

      ecs.removeEntityFromSystem<TestSystem>(e);
      ecs.removeComponentTypeFromEntity<TestComponent>(e);

      // The entity's signature and the system's signature should no longer
      // be compatible at this point.
      REQUIRE_THROWS(ecs.addEntityToSystem<TestSystem>(e));
    }

    SECTION("Removing a component type to an entity should invoke the ECS "
            "module to notify the systems the entity is in about the change "
            "and move the entity to the appropriate systems")
    {
      ecs.registerSystem<TestSystem>();

      const Entity e = ecs.createEntity();
      ecs.addComponentTypeToEntity<TestComponent>(e);

      ecs.addEntityToSystem<TestSystem>(e);

      REQUIRE_NOTHROW(ecs.removeComponentTypeFromEntity<TestComponent>(e));

      // Since the entity should have already been removed from TestSystem
      // after removing TestComponent from the entity.
      REQUIRE_THROWS(ecs.removeEntityFromSystem<TestSystem>(e));
    }
  }

  SECTION("Getting a component type of an entity must be done properly")
  {
    ecs.registerComponentType<TestComponent>();

    SECTION("Gets the component type of an existing entity")
    {
      const Entity e = ecs.createEntity();
      ecs.addComponentTypeToEntity<TestComponent>(e);

      REQUIRE_NOTHROW(([&] () {
        TestComponent& c = ecs.getEntityComponentType<TestComponent>(e);
      }));
    }

    SECTION("Getting the component type of a non-existent entity should cause "
            "an exception")
    {
      const Entity nonExistentEntity = 15;
      REQUIRE_THROWS_AS(
        ecs.getEntityComponentType<TestComponent>(nonExistentEntity),
        NonExistentEntityError);
    }

    SECTION("Getting a component type of an entity that does not have such a "
            "type should cause an exception")
    {
      const Entity e = ecs.createEntity();
      REQUIRE_THROWS_AS(ecs.getEntityComponentType<TestComponent>(e),
                        NoComponentForEntityError);
    }
  }

  SECTION("Registering systems must be done properly")
  {
    ecs.registerComponentType<TestComponent>();
    ecs.registerSystem<TestSystem>();
    REQUIRE_NOTHROW(ecs.getSystem<TestSystem>());

    // TODO: There should be an error when a system has been registered twice.
  }

  SECTION("Getting a system must be done properly")
  {
    SECTION("Gets a registered system")
    {
      ecs.registerComponentType<TestComponent>();
      ecs.registerSystem<TestSystem>();
      REQUIRE_NOTHROW(([&] () {
        TestSystem& system = ecs.getSystem<TestSystem>();
      }));
    }

    SECTION("Getting an unregistered system should cause an exception")
    {
      // Note that, at this point, TestSystem has not yet been registered.
      REQUIRE_THROWS_AS(ecs.getSystem<TestSystem>(),
                        UnregisteredSystemError);
    }
  }

  SECTION("Getting a system's signature must be done properly")
  {
    ecs.registerComponentType<TestComponent>();
    ecs.registerSystem<TestSystem>();
    REQUIRE(ecs.getSystemSignature<TestSystem>() == Signature{1});
  }

  SECTION("Adding an entity to a system must be done properly")
  {
    const Entity e = ecs.createEntity();
    ecs.registerComponentType<TestComponent>();
    ecs.addComponentTypeToEntity<TestComponent>(e);

    TestComponent& c = ecs.getEntityComponentType<TestComponent>(e);
    c.x = 1;

    ecs.registerSystem<TestSystem>();

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
      ecs.addEntityToSystem<TestSystem>(e);
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

  SECTION("Removing an entity from a system should be done properly")
  {
    const Entity e = ecs.createEntity();
    ecs.registerComponentType<TestComponent>();
    ecs.addComponentTypeToEntity<TestComponent>(e);

    TestComponent& c = ecs.getEntityComponentType<TestComponent>(e);
    c.x = 1;

    ecs.registerSystem<TestSystem>();
    ecs.addEntityToSystem<TestSystem>(e);

    SECTION("Removes an existing entity from a system")
    {
      REQUIRE_NOTHROW(ecs.removeEntityFromSystem<TestSystem>(e));

      // Making sure that the removed entity has been indeed removed the system.
      REQUIRE_NOTHROW(ecs.addEntityToSystem<TestSystem>(e));
    }

    SECTION("Removing a non-existent entity from a system should cause an "
            "exception")
    {
      const Entity nonExistentEntity = 15;
      REQUIRE_THROWS_AS(ecs.removeEntityFromSystem<TestSystem>(e),
                        NonExistentEntityError);
    }

    SECTION("Removing the same entity twice from a system should cause an "
            "exception")
    {
      ecs.removeEntityFromSystem<TestSystem>(e);
      REQUIRE_THROWS_AS(ecs.removeEntityFromSystem<TestSystem>(e),
                        UnregisteredEntityError);
    }

    SECTION("Removing an entity that is not in a system should cause an "
            "exception")
    {
      ecs.removeEntityFromSystem<TestSystem>(e);

      // Note that entity, e, at this point is not a member of TestSystem.
      REQUIRE_THROWS_AS(ecs.removeEntityFromSystem<TestSystem>(e),
                        UnregisteredEntityError);
    }
  }
}
