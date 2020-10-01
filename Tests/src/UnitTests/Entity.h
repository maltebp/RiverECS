#pragma once

#include <catch.h>

#include <ECS.h>
#include <ECS/ComponentController.h>

#include "TestComponents.h"
#include "Log.h"




//============================================================================================================================================================================================================
TEST_CASE("Creating, getting, updating and removing Entity", "[entity]") {
	/*	Basic component functionality */

	ECS::Domain domain;
	ECS::Entity* entity;
	ComponentA* comp;
	
	entity = domain.createEntity();

	comp = entity->getComponent<ComponentA>();
	REQUIRE( comp == nullptr);

	comp = entity->addComponent<ComponentA>();

	// Test initial values
	REQUIRE(comp->a == 10);
	REQUIRE(comp->b == 15);

	
	// Update component
	comp = entity->getComponent<ComponentA>();
	REQUIRE(comp != nullptr);
	comp->a = 5;
	comp->b = 30;

	// Checking update
	comp = entity->getComponent<ComponentA>();
	REQUIRE(comp->a == 5);
	REQUIRE(comp->b == 30);

	// Checking after domain clean
	domain.clean();
	comp = entity->getComponent<ComponentA>();
	REQUIRE(comp->a == 5);
	REQUIRE(comp->b == 30);

	
	// Removing component
	entity->removeComponent<ComponentA>();

	comp = entity->getComponent<ComponentA>();
	REQUIRE(comp != nullptr);

	domain.clean();

	comp = entity->getComponent<ComponentA>();
	REQUIRE(comp == nullptr);
}



// ==============================================================================================================================================================================================
TEST_CASE("Multiple components", "[entity]") {
	/*	Checking that an Entity can have multiple components */

	ECS::Domain domain;
	ECS::Entity* entity;


	entity = domain.createEntity();

	entity->addComponent<ComponentA>();
	entity->addComponent<ComponentB>();
	entity->addComponent<ComponentC>();
	entity->addComponent<ComponentD>();
	entity->addComponent<ComponentE>();
	entity->addComponent<ComponentF>();
	entity->addComponent<ComponentG>();
	entity->addComponent<ComponentH>();

	REQUIRE(entity->getComponent<ComponentA>() != nullptr);
	REQUIRE(entity->getComponent<ComponentB>() != nullptr);
	REQUIRE(entity->getComponent<ComponentC>() != nullptr);
	REQUIRE(entity->getComponent<ComponentD>() != nullptr);
	REQUIRE(entity->getComponent<ComponentE>() != nullptr);
	REQUIRE(entity->getComponent<ComponentF>() != nullptr);
	REQUIRE(entity->getComponent<ComponentG>() != nullptr);
	REQUIRE(entity->getComponent<ComponentH>() != nullptr);

	entity->getComponent<ComponentA>()->a = 40;
	entity->getComponent<ComponentB>()->a = 50;
	entity->getComponent<ComponentC>()->a = 60;
	entity->getComponent<ComponentD>()->a = 70;
	entity->getComponent<ComponentE>()->a = 80;
	entity->getComponent<ComponentF>()->a = 90;
	entity->getComponent<ComponentG>()->a = 100;
	entity->getComponent<ComponentH>()->a = 110;

	REQUIRE(entity->getComponent<ComponentA>()->a == 40);
	REQUIRE(entity->getComponent<ComponentB>()->a == 50);
	REQUIRE(entity->getComponent<ComponentC>()->a == 60);
	REQUIRE(entity->getComponent<ComponentD>()->a == 70);
	REQUIRE(entity->getComponent<ComponentE>()->a == 80);
	REQUIRE(entity->getComponent<ComponentF>()->a == 90);
	REQUIRE(entity->getComponent<ComponentG>()->a == 100);
	REQUIRE(entity->getComponent<ComponentH>()->a == 110);

	domain.clean();

	REQUIRE(entity->getComponent<ComponentA>()->a == 40);
	REQUIRE(entity->getComponent<ComponentB>()->a == 50);
	REQUIRE(entity->getComponent<ComponentC>()->a == 60);
	REQUIRE(entity->getComponent<ComponentD>()->a == 70);
	REQUIRE(entity->getComponent<ComponentE>()->a == 80);
	REQUIRE(entity->getComponent<ComponentF>()->a == 90);
	REQUIRE(entity->getComponent<ComponentG>()->a == 100);
	REQUIRE(entity->getComponent<ComponentH>()->a == 110);

	
	entity->removeComponent<ComponentB>();
	entity->removeComponent<ComponentD>();
	entity->removeComponent<ComponentH>();

	REQUIRE(entity->getComponent<ComponentA>() != nullptr);
	REQUIRE(entity->getComponent<ComponentB>() != nullptr);
	REQUIRE(entity->getComponent<ComponentC>() != nullptr);
	REQUIRE(entity->getComponent<ComponentD>() != nullptr);
	REQUIRE(entity->getComponent<ComponentE>() != nullptr);
	REQUIRE(entity->getComponent<ComponentF>() != nullptr);
	REQUIRE(entity->getComponent<ComponentG>() != nullptr);
	REQUIRE(entity->getComponent<ComponentH>() != nullptr);

	domain.clean();

	REQUIRE(entity->getComponent<ComponentA>() != nullptr);
	REQUIRE(entity->getComponent<ComponentB>() == nullptr);
	REQUIRE(entity->getComponent<ComponentC>() != nullptr);
	REQUIRE(entity->getComponent<ComponentD>() == nullptr);
	REQUIRE(entity->getComponent<ComponentE>() != nullptr);
	REQUIRE(entity->getComponent<ComponentF>() != nullptr);
	REQUIRE(entity->getComponent<ComponentG>() != nullptr);
	REQUIRE(entity->getComponent<ComponentH>() == nullptr);
}



// ==============================================================================================================================================================================================
TEST_CASE("General Signature search", "[entity]") {
	/*	Checking that quering entities based on a series of
		components works
	*/
	

	ECS::Domain domain;
	ECS::Entity* entity;

	entity = domain.createEntity();
	entity->addComponent<ComponentA>();
	entity->addComponent<ComponentF>();
	entity->addComponent<ComponentH>();

	entity = domain.createEntity();
	entity->addComponent<ComponentA>();
	entity->addComponent<ComponentB>();

	entity = domain.createEntity();
	entity->addComponent<ComponentA>();
	entity->addComponent<ComponentB>();

	entity = domain.createEntity();
	entity->addComponent<ComponentA>();
	entity->addComponent<ComponentB>();
	entity->addComponent<ComponentC>();

	entity = domain.createEntity();
	entity->addComponent<ComponentA>();
	entity->addComponent<ComponentB>();
	entity->addComponent<ComponentC>();
	entity->addComponent<ComponentD>();
	entity->addComponent<ComponentE>();
	entity->addComponent<ComponentF>();
	entity->addComponent<ComponentG>();

	entity = domain.createEntity();
	entity->addComponent<ComponentA>();
	entity->addComponent<ComponentB>();
	entity->addComponent<ComponentC>();
	entity->addComponent<ComponentD>();

	domain.clean();
	
	int entityCount;
	
	
	entityCount = 0;
	domain.forEachEntity<ComponentA, ComponentB>([&entityCount](auto entity, auto a, auto b) {
		entityCount++;
	});
	REQUIRE(entityCount == 5);

	entityCount = 0;
	domain.forEachEntity<ComponentC>([&entityCount](auto entity, auto c) {
		entityCount++;
		});
	REQUIRE(entityCount == 3);


	entityCount = 0;
	domain.forEachEntity<ComponentC, ComponentF>([&entityCount](auto entity, auto c, auto f) {
		entityCount++;
		});
	REQUIRE(entityCount == 1);

	entityCount = 0;
	domain.forEachEntity<ComponentA>([&entityCount](auto entity, auto a) {
		entityCount++;
		});
	REQUIRE(entityCount == 6);
}




// ==============================================================================================================================================================================================
TEST_CASE("Non-existing Component signature search", "[entity]") {
	/*	Querying entities with components that no entity has yet.
		This would means that the type probably hasn't been
		registered in the system as a Component yet.
	*/


	ECS::Domain domain;
	ECS::Entity* entity;

	entity = domain.createEntity();
	entity->addComponent<ComponentA>();
	entity->addComponent<ComponentB>();
	entity->addComponent<ComponentC>();

	domain.clean();

	int entityCount;


	entityCount = 0;
	domain.forEachEntity<ComponentD>([&entityCount](auto entity, auto a) {
		entityCount++;
		});
	REQUIRE(entityCount == 0);

	entityCount = 0;
	domain.forEachEntity<ComponentE, ComponentF>([&entityCount](auto entity, auto e, auto f) {
		entityCount++;
		});
	REQUIRE(entityCount == 0);

	entityCount = 0;
	domain.forEachEntity<ComponentA, ComponentG>([&entityCount](auto entity, auto c, auto g) {
		entityCount++;
		});
	REQUIRE(entityCount == 0);
}




// ==============================================================================================================================================================================================
TEST_CASE("Same Cycle Create and Destroy", "[entity]") {
	/*	Checking that Entities and Components are handled correctly
		if they both are destroyed in the same Domain cycle (before
		cleaning).
	*/

	ECS::Domain domain;

	// Pre-filling domain with a few entities
	auto entity1 = domain.createEntity();
	entity1->addComponent<ComponentA>();
	entity1->addComponent<ComponentB>();

	auto entity2 = domain.createEntity();
	entity2->addComponent<ComponentA>();
	entity2->addComponent<ComponentC>();
	
	domain.clean();

	int entityCount;
	entityCount = 0;
	domain.forEachEntity<ComponentA>([&entityCount](auto entity, auto a) {
		entityCount++;
		});
	REQUIRE(entityCount == 2);


	SECTION("Create and destroy new entity") {
	
		
		// Creating and destroying new entity
		auto entity3 = domain.createEntity();
		entity3->addComponent<ComponentA>();
		entity3->addComponent<ComponentD>();

		REQUIRE(entity3->getComponent<ComponentA>() != nullptr);
		REQUIRE(entity3->getComponent<ComponentD>() != nullptr);

		domain.destroyEntity(entity3);
		
		REQUIRE(domain.getEntityComponent<ComponentA>(entity3) != nullptr);
		REQUIRE(domain.getEntityComponent<ComponentD>(entity3) != nullptr);

		domain.clean();

		REQUIRE(domain.getEntityComponent<ComponentA>(entity3) == nullptr);
		REQUIRE(domain.getEntityComponent<ComponentD>(entity3) == nullptr);

		entityCount = 0;
		domain.forEachEntity<ComponentD>([&entityCount](auto entity, auto a) {
			entityCount++;
		});
		REQUIRE(entityCount == 0);
	}



	SECTION("Create and destroy component on new entity") {
	
		entity2->addComponent<ComponentD>();
		REQUIRE(entity2->getComponent<ComponentD>() != nullptr);

		domain.removeEntityComponent<ComponentD>(entity2);

		REQUIRE(domain.getEntityComponent<ComponentD>(entity2) != nullptr);

		domain.clean();

		REQUIRE(domain.getEntityComponent<ComponentD>(entity2) == nullptr);

		entityCount = 0;
		domain.forEachEntity<ComponentD>([&entityCount](auto entity, auto a) {
			entityCount++;
			});
		REQUIRE(entityCount == 0);
	}



	SECTION("Create and destroy both component and entity") {

		// Creating and destroying new entity
		auto entity3 = domain.createEntity();
		entity3->addComponent<ComponentA>();
		entity3->addComponent<ComponentD>();

		domain.removeEntityComponent<ComponentD>(entity3);

		REQUIRE(entity3->getComponent<ComponentA>() != nullptr);
		REQUIRE(entity3->getComponent<ComponentD>() != nullptr);

		domain.destroyEntity(entity3);

		REQUIRE(domain.getEntityComponent<ComponentA>(entity3) != nullptr);
		REQUIRE(domain.getEntityComponent<ComponentD>(entity3) != nullptr);

		domain.clean();

		REQUIRE(domain.getEntityComponent<ComponentA>(entity3) == nullptr);
		REQUIRE(domain.getEntityComponent<ComponentD>(entity3) == nullptr);

		entityCount = 0;
		domain.forEachEntity<ComponentD>([&entityCount](auto entity, auto a) {
			entityCount++;
			});
		REQUIRE(entityCount == 0);
	}


}