#include <stdexcept>
#include <string>

#include <catch2/catch.hpp>

#include <planes/engine/ecs/component.hpp>
#include <planes/engine/ecs/entity.hpp>

using namespace planes::engine::ecs;

TEST_CASE("Component Array must be able to manage its components properly ",
          "[ECS | ComponentArray]")
{
  struct TestComponent
  {
    int x;
  };

  const unsigned int numTestEntities = 5;
  EntityManager<numTestEntities> entityManager{};
  ComponentArray<TestComponent> components{};

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
        components.getComponent(entities[0]);
      })());
    }
  }

  SECTION("Getting components of entities should be done properly")
  {
    SECTION("Gets the correct component of entities that are within the "
            "proper range")
    {
      for (int i = 0; i < numTestEntities - 2; i++) {
        // Remember that the last test entity has been deleted. So, we're not
        // going to create a component for it here.
        TestComponent tc{i};
        components.addComponent(entities[i], tc);
      }

      for (int i = 0; i < numTestEntities - 2; i++) {
        TestComponent& tc = components.getComponent(entities[i]);
        REQUIRE(tc.x == i);
      }
    }

    SECTION("Getting the component of an entity that is within the proper "
            "range should give us a pointer or reference to the component")
    {
      components.addComponent(entities[0], TestComponent{});
      REQUIRE_NOTHROW(([&] {
        TestComponent& tc = components.getComponent(entities[0]);
      })());
    }

    SECTION("Getting the component of an entity that is not within the proper "
            "range should cause an exception, but the invalid entity should "
            "be treated as though it is valid")
    {
      // Throwing an exception because of an invalid entity must be handled
      // by the ECS manager.
      REQUIRE_THROWS_AS(components.getComponent(-1), NoComponentForEntityError);
      REQUIRE_THROWS_AS(components.getComponent(entityManager.kMaxNumEntities),
                        NoComponentForEntityError);
    }

    SECTION("Getting the component of an entity that does not exist should "
            "cause an exception, but the non-existent entity should be "
            "treated as though it exists (even though it isn't, and it "
            "shouldn't be forced to exist)")
    {
      // An entity that does not exist is typically an entity that has been
      // deleted.
      REQUIRE_THROWS_AS(components.getComponent(deletedEntity),
                        NoComponentForEntityError);
    }

    SECTION("Getting a component of an entity which does not have a component "
            "in the component array should cause an exception")
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

    SECTION("Deleting a component of an entity that does not have a component "
            "in the component array should cause an exception")
    {
      REQUIRE_THROWS_AS(components.deleteComponent(entities[0]),
                        NoComponentForEntityError);
    }
  }

  SECTION("Notification of a deletion of an entity must be handled properly")
  {
    SECTION("Receiving a notification should invoke deletion of the component "
            "of the deleted entity, if and only if the entity has such a "
            "component")
    {
      const Entity deletedEntity = entities[0];
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

      for (int i = 0; i < numTestEntities - 1; i++) {
        // Remember that the last test entity has been deleted. So, we're not
        // going to create a component for it here.
        components.addComponent(entities[i], TestComponent{});
      }

      components.notifyEntityDeleted(deletedEntity);

      for (int i = 0; i < numTestEntities - 1; i++) {
        REQUIRE_NOTHROW(components.getComponent(entities[i]));
      }
    }
  }
}

TEST_CASE("ComponentManager must be able to manage components properly ",
          "[ECS | ComponentManager]")
{
  EntityManager<5> entityManager{};
  
  int numTestEntities = 5;
  Entity entities[numTestEntities];
  for (int i = 0; i < numTestEntities; i++) {
    entities[i] = entityManager.createEntity();
  }

  Entity deletedEntity = entities[numTestEntities - 1];
  entityManager.deleteEntity(deletedEntity);

  struct TestComponent0
  {
    int x;
  };

  struct TestComponent1
  {
    int x;
    int y;
  };

  ComponentManager componentManager{};

  SECTION("Registering a component type must be done properly")
  {
    SECTION("Registers a component type properly")
    {
      REQUIRE_NOTHROW(componentManager.registerComponentType<TestComponent0>());

      Entity e = entities[0];
      componentManager.addComponentType<TestComponent0>(e);

      REQUIRE(componentManager.getComponentTypeIndex<TestComponent0>() == 0);
    }

    SECTION("Registers two component types properly")
    {
      REQUIRE_NOTHROW(componentManager.registerComponentType<TestComponent0>());
      REQUIRE_NOTHROW(componentManager.registerComponentType<TestComponent1>());
      
      Entity e = entities[0];
      componentManager.addComponentType<TestComponent0>(e);
      componentManager.addComponentType<TestComponent1>(e);

      REQUIRE(componentManager.getComponentTypeIndex<TestComponent0>() == 0);
      REQUIRE(componentManager.getComponentTypeIndex<TestComponent1>() == 1);
    }
  }

  SECTION("Getting the index of component type must be done properly")
  {
    SECTION("Gets the index of a component type properly")
    {
      componentManager.registerComponentType<TestComponent0>();
      REQUIRE(componentManager.getComponentTypeIndex<TestComponent0>() == 0);
    }

    SECTION("Gets the index of two component types properly")
    {
      componentManager.registerComponentType<TestComponent0>();
      componentManager.registerComponentType<TestComponent1>();

      REQUIRE(componentManager.getComponentTypeIndex<TestComponent0>() == 0);
      REQUIRE(componentManager.getComponentTypeIndex<TestComponent1>() == 1);
    }

    SECTION("Getting the index of a component type that has not been "
            "registered yet")
    {
      REQUIRE_THROWS_AS(
        componentManager.getComponentTypeIndex<TestComponent0>(),
        UnregisteredComponentTypeError);
    }
  }

  SECTION("Getting a component of an entity must be done properly")
  {
    componentManager.registerComponentType<TestComponent0>();

    SECTION("Gets the correct component of entities that are within the "
            "proper range")
    {
      for (int i = 0; i < numTestEntities - 2; i++) {
        // Remember that the last test entity has been deleted. So, we're not
        // going to create a component for it here.
        componentManager.addComponentType<TestComponent0>(entities[i]);
      }

      REQUIRE_NOTHROW(([&] {
        for (int i = 0; i < numTestEntities - 2; i++) {
          TestComponent0& tc = componentManager
                                 .getComponent<TestComponent0>(entities[i]);
          tc.x = i;
        }

        for (int i = 0; i < numTestEntities - 2; i++) {
          TestComponent0& tc = componentManager
                                 .getComponent<TestComponent0>(entities[i]);
          REQUIRE(tc.x == i);
        }
      }));
    }

    SECTION("Getting the component of an entity that is within the proper "
            "range should give us a pointer or reference to the component")
    {
      const Entity e = entities[0];
      componentManager.addComponentType<TestComponent0>(e);
      REQUIRE_NOTHROW(([&] {
        TestComponent0& tc = componentManager.getComponent<TestComponent0>(e);
      }));
    }

    SECTION("Gets the component of an entity that is not within the proper "
            "range properly")
    {
      // Throwing an entity-related exception because of an invalid entity
      // must be handled by the ECS manager.
      componentManager.addComponentType<TestComponent0>(-1);
      componentManager.addComponentType<TestComponent0>(
        entityManager.kMaxNumEntities);
      REQUIRE_NOTHROW(([&] {
        componentManager.getComponent<TestComponent0>(-1);
        componentManager
          .getComponent<TestComponent0>(entityManager.kMaxNumEntities);
      }));
    }

    SECTION("Gets the component of a non-existent entity properly")
    {
      // An entity that does not exist is typically an entity that has been
      // deleted.
      componentManager.addComponentType<TestComponent0>(deletedEntity);
      REQUIRE_NOTHROW(componentManager
                        .getComponent<TestComponent0>(deletedEntity));
    }

    SECTION("Getting a component of an entity which does not have a component "
            "that is being obtained should cause an exception")
    {
      componentManager.registerComponentType<TestComponent1>();

      const Entity e = entities[0];
      componentManager.addComponentType<TestComponent0>(e);
      REQUIRE_THROWS_AS(componentManager.getComponent<TestComponent1>(e),
                        NoComponentForEntityError);
    }

    SECTION("Getting a component of an entity, whose type has not been "
            "registered yet should cause an exception")
    {
      // In terms of exception raising, an UnregisteredComponentTypeError
      // exception will first be raised. Then, if that isn't raised, then
      // a NoComponentForEntityError will be raised, if conditions are
      // met for such an exception to be raised.

      // We need a fresh component manager for this test case, one where
      // TestComponent0 has not yet been registered.
      ComponentManager componentManager{};

      const Entity e = entities[0];
      REQUIRE_THROWS_AS(componentManager.getComponent<TestComponent0>(e),
                        UnregisteredComponentTypeError);
    }
  }

  SECTION("Adding a component type to an entity must be done properly")
  {
    componentManager.registerComponentType<TestComponent0>();

    SECTION("Adds a component type to an entity that is within the proper "
            "range properly")
    {
      const Entity e = entities[0];

      REQUIRE_NOTHROW(componentManager.addComponentType<TestComponent0>(e));
      REQUIRE_NOTHROW(componentManager.getComponent<TestComponent0>(e));
    }

    SECTION("Adds a component type to an entity even though it is not within "
            "the proper range properly")
    {
      // Throwing an entity-related exception because of an invalid entity
      // must be handled by the ECS manager.
      REQUIRE_NOTHROW(componentManager.addComponentType<TestComponent0>(-1));
      REQUIRE_NOTHROW(componentManager.getComponent<TestComponent0>(-1));

      REQUIRE_NOTHROW(componentManager.addComponentType<TestComponent0>(
          entityManager.kMaxNumEntities));
      REQUIRE_NOTHROW(componentManager.getComponent<TestComponent0>(
          entityManager.kMaxNumEntities));
    }

    SECTION("Adds a component type to a non-existent entity properly")
    {
      REQUIRE_NOTHROW(
        componentManager.addComponentType<TestComponent0>(deletedEntity));
      REQUIRE_NOTHROW(
        componentManager.getComponent<TestComponent0>(deletedEntity));
    }

    SECTION("Adding a component type to an entity without registering "
            "the component type first should cause an exception")
    {
      // We need a fresh component manager for this test case, one where
      // TestComponent0 has not yet been registered.
      ComponentManager componentManager{};

      const Entity e = entities[0];
      REQUIRE_THROWS_AS(componentManager.addComponentType<TestComponent0>(e),
                        UnregisteredComponentTypeError);
    }
  }

  SECTION("Deleting a component from an entity must be done properly")
  {
    componentManager.registerComponentType<TestComponent0>();

    SECTION("Deletes the component of an entity that is within the proper "
            "range")
    {
      const Entity e = entities[0];
      componentManager.addComponentType<TestComponent0>(e);
      REQUIRE_NOTHROW(componentManager.deleteComponentType<TestComponent0>(e));
    }

    SECTION("Deletes the component of an entity that is within the proper "
            "range")
    {
      componentManager.addComponentType<TestComponent0>(-1);
      REQUIRE_NOTHROW(componentManager.deleteComponentType<TestComponent0>(-1));

      componentManager.addComponentType<TestComponent0>(
        entityManager.kMaxNumEntities);
      REQUIRE_NOTHROW(componentManager.deleteComponentType<TestComponent0>(
        entityManager.kMaxNumEntities));
    }

    SECTION("Deletes the component of a non-existent entity")
    {
      componentManager.addComponentType<TestComponent0>(deletedEntity);
      REQUIRE_NOTHROW(
        componentManager.deleteComponentType<TestComponent0>(deletedEntity));
    }

    SECTION("Deleting a component of an entity that the entity does not have "
            "should cause an exception")
    {
      const Entity e = entities[0];
      REQUIRE_THROWS_AS(componentManager.deleteComponentType<TestComponent0>(e),
                        NoComponentForEntityError);
    }

    SECTION("Deleting a component type to an entity without registering "
            "the component type first should cause an exception")
    {
      const Entity e = entities[0];
      REQUIRE_THROWS_AS(componentManager.deleteComponentType<TestComponent1>(e),
                        UnregisteredComponentTypeError);
    }
  }

  SECTION("Notification of a deletion of an entity must be done properly")
  {
    componentManager.registerComponentType<TestComponent0>();
    
    SECTION("Receiving a notification should invoke deletion of the "
            "components of the deleted entity")
    {
      const Entity deletedEntity = entities[0];
      componentManager.addComponentType<TestComponent0>(deletedEntity);
      entityManager.deleteEntity(deletedEntity);

      componentManager.notifyEntityDeleted(deletedEntity);
      REQUIRE_THROWS_AS(componentManager
                          .getComponent<TestComponent0>(deletedEntity),
                        NoComponentForEntityError);
    }

    SECTION("Receiving a notification for an entity does not affect the "
            "components of other entities")
    {
      for (int i = 0; i < numTestEntities - 1; i++) {
        // Remember that the last test entity has been deleted. So, we're not
        // going to create a component for it here.
        const Entity e = entities[i];
        componentManager.addComponentType<TestComponent0>(e);
      }

      componentManager.notifyEntityDeleted(deletedEntity);

      for (int i = 0; i < numTestEntities - 1; i++) {
        const Entity e = entities[i];
        REQUIRE_NOTHROW(componentManager.getComponent<TestComponent0>(e));
      }
    }
  }
}
