#pragma once

#include <ECS.h>
#include <ECS/ComponentController.h>

#include "Components/Components.h"

#include "Log.h"


struct TestComponent : ECS::Component {
	unsigned value = 0;
};




void run() {

	LOG("Starting sandbox");

	ECS::Domain domain;
	std::vector<ECS::Entity*> entities;

	auto entity = domain.createEntity();
	entities.push_back(entity);

	auto component = entity->addComponent<TestComponent>();

	component->value = 10;


	domain.clean();

	LOG(entity->getComponent<TestComponent>()->value);

	entity->removeComponent<TestComponent>();
	
	if( entity->getComponent<TestComponent>() != nullptr )
		LOG("Has component");
	else
		LOG("Doesn't have component");

	domain.clean();
	
	if( entity->getComponent<TestComponent>() != nullptr )
		LOG("Has component");
	else
		LOG("Doesn't have component");

}

int start(int argc, char* argv[]) {
	run();
	

	return 0;
}