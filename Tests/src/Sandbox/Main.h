#pragma once

#include <ECS.h>
#include <ECS/ComponentController.h>

#include "Components/Components.h"

#include "Log.h"


struct TestComponent : ECS::Component {
	unsigned value = 0;
};

struct TestComponent2 : ECS::Component {
	unsigned value = 0;
};

struct TestComponent3 : ECS::Component {
	unsigned value = 0;
};


void run() {

	LOG("Starting sandbox");

	ECS::Domain domain;
	std::vector<ECS::Entity*> entities;

	auto entity = domain.createEntity();
	entities.push_back(entity);

	entity->addComponent<TestComponent>();
	auto component = entity->addComponent<TestComponent2>();
	component->value = 10;

	domain.createEntity()->addComponent<TestComponent>()->value = 15;
	domain.createEntity()->addComponent<TestComponent2>()->value = 15;
	domain.createEntity();
	domain.createEntity()->addComponent<TestComponent>()->value = 20;
	domain.createEntity()->addComponent<TestComponent>()->value = 25;
	domain.createEntity();
	domain.createEntity()->addComponent<TestComponent>()->value = 30;

	domain.clean();


	domain.forMatchingEntities<TestComponent, TestComponent2>([](ECS::Entity* e, TestComponent* c, TestComponent2* c2) {
		LOG(c->value);
		LOG(c2->value);
	});

}

int start(int argc, char* argv[]) {
	run();
	

	return 0;
}