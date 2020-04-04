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

  // TODO: Systems must no longer accept signature as constructor arguments.

  SECTION("Registering the required component types of a system should be "
          "done properly")
  {
    System system{componentManager};

    // Note: registerRequiredComponentType() must have a protected access level.
    REQUIRE_NOTHROW(system.registerRequiredComponentType<TestComponent>());

    REQUIRE(system.getSignature() == Signature{1});
  }

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

  SECTION("Gets the system's signature properly")
  {
    REQUIRE(system.getSignature() == Signature{1});
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

  SECTION("Checking existence of an entity in a system should be done properly")
  {
    SECTION("Returns true if an entity has been added to a system")
    {
      const Entity e = entities[0];
      system.addEntity(e);

      REQUIRE(system.hasEntity(e));
    }

    SECTION("Returns false if an entity has not been added to a system")
    {
      const Entity e = entities[0];
      REQUIRE_FALSE(system.hasEntity(e));
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

TEST_CASE("SystemManager must be able to manage systems properly "
          "[ECS | SystemManager]")
{
  struct TestComponent0
  {
    int x;
  };

  struct TestComponent1
  {
    int y;
  };

  struct TestComponent2
  {
    int z;
  };

  ComponentManager componentManager{};
  componentManager.registerComponentType<TestComponent0>();
  componentManager.registerComponentType<TestComponent1>();
  componentManager.registerComponentType<TestComponent2>();

  class TestSystem0 : public System
  {
  public:
    TestSystem0(ComponentManager& componentManager)
      : System(Signature{1}, componentManager) {}

    void update() override
    {
      for (const Entity e : this->entities) {
        TestComponent0& c = this->componentManager
                                 .getComponent<TestComponent0>(e);
        if (c.x == 1) {
          c.x = 0;
        }
      }
    }
  };

  class TestSystem1 : public System
  {
  public:
    TestSystem1(ComponentManager& componentManager)
      : System(Signature{2}, componentManager) {}

    void update() override
    {
      for (const Entity e : this->entities) {
        TestComponent1& c = this->componentManager
                                 .getComponent<TestComponent1>(e);
        if (c.y == 1) {
          c.y = 2;
        }
      }
    }
  };

  class TestSystem2 : public System
  {
  public:
    TestSystem2(ComponentManager& componentManager)
      : System(Signature{4}, componentManager) {}

    void update() override
    {
      for (const Entity e : this->entities) {
        TestComponent2& c = this->componentManager
                                 .getComponent<TestComponent2>(e);
        if (c.z == 1) {
          c.z = 2;
        }
      }
    }
  };

  class UnregisteredSystem : public System
  {
  public:
    UnregisteredSystem(ComponentManager& componentManager)
      : System(Signature{0}, componentManager) {}

    void update() override {}
  };

  const unsigned int numTestEntities = 7;
  EntityManager<numTestEntities> entityManager{};

  Entity entities0[numTestEntities / 2];
  for (int i = 0; i < (numTestEntities / 2); i++) {
    entities0[i] = entityManager.createEntity();
    componentManager.addComponentType<TestComponent0>(entities0[i]);
  }

  Entity entities1[numTestEntities / 2];
  for (int i = 0; i < (numTestEntities / 2); i++) {
    entities1[i] = entityManager.createEntity();
    componentManager.addComponentType<TestComponent1>(entities1[i]);
  }

  SECTION("Registering a system must be done properly")
  {
    SystemManager systemManager{componentManager};

    SECTION("Registers a system properly")
    {
      REQUIRE_NOTHROW(systemManager.registerSystem<TestSystem0>());

      const Entity e = entities0[0];
      systemManager.addEntity<TestSystem0>(e);

      TestComponent0& tc = componentManager.getComponent<TestComponent0>(e);
      tc.x = 1;

      systemManager.getSystem<TestSystem0>().update();

      REQUIRE(tc.x == 0);
    }

    SECTION("Registers two systems properly")
    {
      REQUIRE_NOTHROW(systemManager.registerSystem<TestSystem0>());
      REQUIRE_NOTHROW(systemManager.registerSystem<TestSystem1>());

      const Entity e0 = entities0[0];
      const Entity e1 = entities1[0];

      systemManager.addEntity<TestSystem0>(e0);
      systemManager.addEntity<TestSystem1>(e1);

      TestComponent0& tc0 = componentManager.getComponent<TestComponent0>(e0);
      TestComponent1& tc1 = componentManager.getComponent<TestComponent1>(e1);

      tc0.x = 1;
      tc1.y = 1;

      systemManager.getSystem<TestSystem0>().update();
      systemManager.getSystem<TestSystem1>().update();

      REQUIRE(tc0.x == 0);
      REQUIRE(tc1.y == 2);
    }
  }

  SECTION("Getting a system must be done properly")
  {
    SystemManager systemManager{componentManager};
    systemManager.registerSystem<TestSystem0>();

    SECTION("Gets the correct system")
    {
      for (const Entity e : entities0) {
        TestComponent0& tc = componentManager.getComponent<TestComponent0>(e);
        systemManager.addEntity<TestSystem0>(e);
        tc.x = 1;
      }

      TestSystem0& system = systemManager.getSystem<TestSystem0>();
      system.update();

      for (const Entity e : entities0) {
        TestComponent0& tc = componentManager.getComponent<TestComponent0>(e);
        REQUIRE(tc.x == 0);
      }
    }

    SECTION("Getting a system should give us a pointer or reference to the "
            "system")
    {
      REQUIRE_NOTHROW(([&] {
        TestSystem0& system = systemManager.getSystem<TestSystem0>();
      }));
    }

    SECTION("Getting an unregistered system should cause an exception")
    {
      REQUIRE_THROWS_AS(systemManager.getSystem<UnregisteredSystem>(),
                        UnregisteredSystemError);
    }
  }

  SECTION("Getting the signature of a system must be done properly")
  {
    SystemManager systemManager{componentManager};
    systemManager.registerSystem<TestSystem0>();

    SECTION("Gets the system signature properly")
    {
      REQUIRE(systemManager.getSignature<TestSystem0>() == Signature{1});
    }

    SECTION("Getting the signature of an unregistered system should cause an "
            "exception")
    {
      REQUIRE_THROWS_AS(systemManager.getSignature<UnregisteredSystem>()
                          == Signature{4},
                        UnregisteredSystemError);
    }
  }

  SECTION("Adding an entity to a system must be done properly")
  {
    SystemManager systemManager{componentManager};
    systemManager.registerSystem<TestSystem0>();
    systemManager.registerSystem<TestSystem1>();
    systemManager.registerSystem<TestSystem2>();

    SECTION("Adds an entity")
    {
      const Entity e = entities0[0];
      TestComponent0& c = componentManager.getComponent<TestComponent0>(e);
      c.x = 1;

      REQUIRE_NOTHROW(systemManager.addEntity<TestSystem0>(e));

      TestSystem0& system = systemManager.getSystem<TestSystem0>();
      system.update();

      REQUIRE(c.x == 0);
    }

    SECTION("Adds two entities")
    {
      const Entity e0 = entities0[0];
      const Entity e1 = entities1[0];

      TestComponent0& c0 = componentManager.getComponent<TestComponent0>(e0);
      TestComponent1& c1 = componentManager.getComponent<TestComponent1>(e1);

      c0.x = 1;
      c1.y = 1;

      REQUIRE_NOTHROW(systemManager.addEntity<TestSystem0>(e0));
      REQUIRE_NOTHROW(systemManager.addEntity<TestSystem1>(e1));

      TestSystem0& system0 = systemManager.getSystem<TestSystem0>();
      TestSystem1& system1 = systemManager.getSystem<TestSystem1>();

      system0.update();
      system1.update();

      REQUIRE(c0.x == 0);
      REQUIRE(c1.y == 2);
    }

    SECTION("Adding the same entity twice should cause an exception")
    {
      const Entity e = entities0[0];
      systemManager.addEntity<TestSystem0>(e);
      REQUIRE_THROWS_AS(systemManager.addEntity<TestSystem0>(e),
                        EntityAlreadyExistsError);
    }

    SECTION("Adding an entity to an unregistered system should cause an "
            "exception")
    {
      const Entity e = entities0[0];
      REQUIRE_THROWS_AS(systemManager.addEntity<UnregisteredSystem>(e),
                        UnregisteredSystemError);
    }

    SECTION("Adding an entity without specifying a system should add it to "
            "all systems that match the entity's signature")
    {
      const Entity e = entityManager.createEntity();
      const Signature signature{3};
      componentManager.addComponentType<TestComponent0>(e);
      componentManager.addComponentType<TestComponent1>(e);

      TestComponent0& tc0 = componentManager.getComponent<TestComponent0>(e);
      TestComponent1& tc1 = componentManager.getComponent<TestComponent1>(e);

      tc0.x = 1;
      tc1.y = 1;

      REQUIRE_NOTHROW(systemManager.addEntity(e, signature));

      TestSystem0& system0 = systemManager.getSystem<TestSystem0>();
      TestSystem1& system1 = systemManager.getSystem<TestSystem1>();

      system0.update();
      system1.update();

      REQUIRE(tc0.x == 0);
      REQUIRE(tc1.y == 2);

      TestSystem2& system2 = systemManager.getSystem<TestSystem2>();

      // No exception would be thrown here if the entity weren't added to
      // TestSystem2. If it was added, then an exception would be thrown since
      // the entity doesn't have a TestComponent2, which the system works with.
      REQUIRE_NOTHROW(system2.update());
    }
  }

  SECTION("Removing an entity from a system must be done properly")
  {
    SystemManager systemManager{componentManager};
    systemManager.registerSystem<TestSystem0>();
    systemManager.registerSystem<TestSystem1>();
    systemManager.registerSystem<TestSystem2>();

    SECTION("Removes an entity that has been added")
    {
      const Entity e = entities0[0];
      TestComponent0& c = componentManager.getComponent<TestComponent0>(e);
      c.x = 1;

      systemManager.addEntity<TestSystem0>(e);

      REQUIRE_NOTHROW(systemManager.removeEntity<TestSystem0>(e));

      TestSystem0& system = systemManager.getSystem<TestSystem0>();
      system.update();

      REQUIRE(c.x != 0);
    }

    SECTION("Removes two entities that have been added")
    {
      const Entity e0 = entities0[0];
      const Entity e1 = entities1[0];

      TestComponent0& c0 = componentManager.getComponent<TestComponent0>(e0);
      TestComponent1& c1 = componentManager.getComponent<TestComponent1>(e1);

      c0.x = 1;
      c1.y = 1;

      systemManager.addEntity<TestSystem0>(e0);
      systemManager.addEntity<TestSystem1>(e1);

      REQUIRE_NOTHROW(systemManager.removeEntity<TestSystem0>(e0));
      REQUIRE_NOTHROW(systemManager.removeEntity<TestSystem1>(e1));

      TestSystem0& system0 = systemManager.getSystem<TestSystem0>();
      TestSystem1& system1 = systemManager.getSystem<TestSystem1>();

      system0.update();
      system1.update();

      REQUIRE(c0.x != 0);
      REQUIRE(c1.y != 2);
    }

    SECTION("Removing an entity that has not been added should cause an "
            "exception")
    {
      const Entity e = entities0[0];
      REQUIRE_THROWS_AS(systemManager.removeEntity<TestSystem0>(e),
                        UnregisteredEntityError);
    }

    SECTION("Removing an entity that was already removed should cause an "
            "exception")
    {
      const Entity e = entities0[0];
      systemManager.addEntity<TestSystem0>(e);
      systemManager.removeEntity<TestSystem0>(e);

      REQUIRE_THROWS_AS(systemManager.removeEntity<TestSystem0>(e),
                        UnregisteredEntityError);
    }

    SECTION("Removing an entity from an unregistered system should cause an "
            "exception")
    {
      const Entity e = entities0[0];
      REQUIRE_THROWS_AS(systemManager.removeEntity<UnregisteredSystem>(e),
                        UnregisteredSystemError);
    }

    SECTION("Removing an entity without specifying a system should remove it to "
            "all systems that match the entity's signature")
    {
      const Entity e = entityManager.createEntity();
      const Signature signature{3};

      componentManager.addComponentType<TestComponent0>(e);
      componentManager.addComponentType<TestComponent1>(e);

      TestComponent0& tc0 = componentManager.getComponent<TestComponent0>(e);
      TestComponent1& tc1 = componentManager.getComponent<TestComponent1>(e);

      tc0.x = 1;
      tc1.y = 1;

      systemManager.addEntity(e, signature);
      REQUIRE_NOTHROW(systemManager.removeEntity(e));

      TestSystem0& system0 = systemManager.getSystem<TestSystem0>();
      TestSystem1& system1 = systemManager.getSystem<TestSystem1>();

      system0.update();
      system1.update();

      REQUIRE(tc0.x != 0);
      REQUIRE(tc1.y != 2);

      // No need to check if the entity has been added to TestSystem2 since that
      // check is handled by another test. We are assuming that systemManager's
      // addEntity() works perfectly. This means that systems that does not have
      // a signature that matches the entity's will never have the entity added
      // to them. A removal of an entity can only be done if there was an
      // addition in the first place. Thus, we only need to perform checks for
      // successful entity removal in systems where the entity was added.
    }
  }

  SECTION("Notification of a signature change of an entity must be done "
          "properly")
  {
    SystemManager systemManager{componentManager};
    systemManager.registerSystem<TestSystem0>();
    systemManager.registerSystem<TestSystem1>();
    systemManager.registerSystem<TestSystem2>();

    SECTION("Receiving a notification should invoke removal of an entity "
            "from systems that it used to be in, and addition of an entity "
            "to systems that match its signature")
    {
      const Entity e0 = entities0[0];
      const Entity e1 = entities1[0];
      const Signature newSignature0{2};
      const Signature newSignature1{1};

      systemManager.addEntity<TestSystem0>(e0);
      systemManager.addEntity<TestSystem1>(e1);

      componentManager.addComponentType<TestComponent0>(e0);
      componentManager.addComponentType<TestComponent1>(e1);

      // Essentially swap the components of the two entities.
      systemManager.notifyEntitySignatureChanged(e0, newSignature0);
      systemManager.notifyEntitySignatureChanged(e1, newSignature1);

      TestSystem0& system0 = systemManager.getSystem<TestSystem0>();
      TestSystem1& system1 = systemManager.getSystem<TestSystem1>();

      REQUIRE(system0.hasEntity(e1));
      REQUIRE(system1.hasEntity(e0));
      REQUIRE_FALSE(system0.hasEntity(e0));
      REQUIRE_FALSE(system1.hasEntity(e1));
    }

    SECTION("Receiving a notification should not affect other entities")
    {
      const Entity e0 = entities0[0];
      const Entity e1 = entities0[1];
      const Entity e2 = entities1[0];

      const Signature newSignature{1};

      // Assume e0 is originally handled by TestSystem1. The change in
      // signature will cause e0 to now be handled by TestSystem0.
      systemManager.addEntity<TestSystem1>(e0);
      systemManager.addEntity<TestSystem0>(e1);
      systemManager.addEntity<TestSystem1>(e2);

      systemManager.notifyEntitySignatureChanged(e0, newSignature);

      // e0 will be evicted from TestSystem1 at this point, and placed to
      // TestSystem0.

      TestSystem0& system0 = systemManager.getSystem<TestSystem0>();
      TestSystem1& system1 = systemManager.getSystem<TestSystem1>();

      REQUIRE(system0.hasEntity(e0));
      REQUIRE(system0.hasEntity(e1));
      REQUIRE(system1.hasEntity(e2));
      REQUIRE_FALSE(system1.hasEntity(e0));
    }
  }
}
