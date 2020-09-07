#pragma once

#include <catch.h>

#include "Log.h"

#include <ECS.h>
#include <ECS/ComponentController.h>

struct TestComponent : public ECS::Component {
	int i;
	char c;
	double d;
};

// Check if Controller's components matches expected components
void checkEntities(ECS::ComponentController<TestComponent>& controller, std::vector<ECS::Entity*>& entities, std::unordered_map<ECS::Entity*, TestComponent>& expectedComponents) {
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

	ECS::Domain domain;   
	ECS::ComponentController<TestComponent> controller;

	// Setup entities
	const auto NUM_ENTITIES = 90;
	std::vector<ECS::Entity*> entities;
	std::unordered_map<ECS::Entity*, TestComponent> expectedComponents;

	for( int i = 0; i < NUM_ENTITIES; i++ ) {
		ECS::Entity* entity = domain.createEntity();
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
}