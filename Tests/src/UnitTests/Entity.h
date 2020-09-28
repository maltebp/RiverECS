#pragma once


#include <catch.h>

#include "TestComponents.h"
#include "Log.h"

#include <ECS.h>
#include <ECS/ComponentController.h>



TEST_CASE("Creating, getting, updating and removing Entity", "[entity]") {


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



TEST_CASE("Multiple components", "[entity]") {


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








TEST_CASE("Signature search", "[entity]") {
	
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