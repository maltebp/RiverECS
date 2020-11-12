#pragma once

#include <string>
#include <catch.h>

#include "Log.h"

#include <ECS.h>
#include <ECS/ComponentController.h>

struct TestComponent : public River::ECS::Component {
	int i;
	char c;
	double d;
};

struct DefaultComponent : public River::ECS::Component {
	int i = 50;
	std::string s = "Default";
};

// Check if Controller's components matches expected components
void checkEntities(River::ECS::ComponentController<TestComponent>& controller, std::vector<River::ECS::Entity*>& entities, std::unordered_map<River::ECS::Entity*, TestComponent>& expectedComponents) {
	for( int i = 0; i < entities.size(); i++ ) {
		auto entity = entities.at(i);
		auto component = controller.getComponent(entity);
		auto expected = expectedComponents[entity];

		REQUIRE(component->i == expected.i);
		REQUIRE(component->c == expected.c);
		REQUIRE(component->d == expected.d);
	}
}


TEST_CASE("Component Controller Tests", "[component_controller]") {

	River::ECS::Domain domain;   
	River::ECS::ComponentController<TestComponent> controller;

	// Setup entities
	const auto NUM_ENTITIES = 90;
	std::vector<River::ECS::Entity*> entities;
	std::unordered_map<River::ECS::Entity*, TestComponent> expectedComponents;

	for( int i = 0; i < NUM_ENTITIES; i++ ) {
		River::ECS::Entity* entity = domain.createEntity();
		entities.push_back(entity);

		auto component = controller.createComponent(entity);
		component->i = i;
		component->c = i;
		component->d = i;
		expectedComponents[entity] = *component;
	}

	SECTION("Modify") {
		// Modify values
		controller.getComponent(entities.at(1))->i = 999;
		expectedComponents[entities.at(1)].i = 999;

		controller.getComponent(entities.at(10))->i = 1234;
		expectedComponents[entities.at(10)].i = 1234;

		controller.getComponent(entities.at(57))->i = 350;
		expectedComponents[entities.at(57)].i = 350;


		SECTION("Pre-cleanup") {
			checkEntities(controller, entities, expectedComponents);
		}

		controller.clean();

		SECTION("Post-cleanup") {
			checkEntities(controller, entities, expectedComponents);
		}
	}


	SECTION("Delete") {
		checkEntities(controller, entities, expectedComponents);

		// Delete entities
		auto entityToDelete = entities.at(55);
		entities.erase(entities.begin() + 55);
		expectedComponents.erase(entityToDelete);
		controller.deleteComponent(entityToDelete);

		REQUIRE(controller.getComponent(entityToDelete) != nullptr);

		checkEntities(controller, entities, expectedComponents);

		controller.clean();
		REQUIRE(controller.getComponent(entityToDelete) == nullptr);

		checkEntities(controller, entities, expectedComponents);
	}

	SECTION("Delete - Double clean") {
		// Clean the controller twice

		checkEntities(controller, entities, expectedComponents);

		// Delete entities
		auto entityToDelete = entities.at(55);
		entities.erase(entities.begin() + 55);
		expectedComponents.erase(entityToDelete);
		controller.deleteComponent(entityToDelete);

		REQUIRE(controller.getComponent(entityToDelete) != nullptr);

		checkEntities(controller, entities, expectedComponents);

		controller.clean();
		REQUIRE(controller.getComponent(entityToDelete) == nullptr);

		controller.clean();
		REQUIRE(controller.getComponent(entityToDelete) == nullptr);

		checkEntities(controller, entities, expectedComponents);
	}
}


// ================================================================================================================================================================================================

TEST_CASE("Component is reset using default constructor on construction", "[component_controller]") {

	River::ECS::Domain domain;
	River::ECS::ComponentController<DefaultComponent> controller;

	auto entity = domain.createEntity();

	// Create component
	auto component = controller.createComponent(entity);

	// Test that initial values (before it's moved to primary list)
	// are correct
	REQUIRE(component->i == 50);
	REQUIRE(component->s == "Default");

	controller.clean();

	// Test that values after being moved to primary list
	// are correct
	component = controller.getComponent(entity);
	REQUIRE(component->i == 50);
	REQUIRE(component->s == "Default");

	// Change values of component
	component->i = 100; 
	component->s = "NotDefault";

	controller.clean();

	// Tests values are changed
	component = controller.getComponent(entity);
	REQUIRE(component->i == 100);
	REQUIRE(component->s == "NotDefault");

	// Delete and create new component ---------------------------
	controller.deleteComponent(entity);
	controller.clean();
	component = controller.createComponent(entity);

	// Test default values before moving to primary list
	REQUIRE(component->i == 50);
	REQUIRE(component->s == "Default");

	controller.clean();

	// Test that values after being moved to primary list
	component = controller.getComponent(entity);
	REQUIRE(component->i == 50);
	REQUIRE(component->s == "Default");


}