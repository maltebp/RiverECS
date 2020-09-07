#pragma once

#include <ECS.h>
#include <ECS/ComponentController.h>

#include "Components/Components.h"

#include "Log.h"
#

struct TestComponent {
	unsigned value = 0;
};




void run() {

	LOG("Starting sandbox");

	ECS::Domain domain;
	ECS::ComponentController<TestComponent> componentController;
	std::vector<ECS::Entity*> entities;

	for( int i = 0; i < 50; i++ ) {
		ECS::Entity* entity = domain.createEntity();

		auto component = componentController.createComponent(entity);
		component->value = i;
		entities.push_back(entity);
	}

	// Delete entity
	/*{
		auto entity = entities.at(10);
		componentController.deleteComponent(entity);

		entity = entities.at(30);
		componentController.deleteComponent(entity);

		componentController.clean();
	}*/


	for( auto entity : entities ) {
		auto component = componentController.getComponent(entity);
		if( component == nullptr ) continue;

		LOG("");
		//LOG("component.id = " << component->id);
		LOG("component.value = " << component->value);
	}
}

int start(int argc, char* argv[]) {
	run();
	run();
	

	return 0;
}