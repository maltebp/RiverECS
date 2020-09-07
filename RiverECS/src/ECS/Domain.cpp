#include "Domain.h"

#include "Entity.h"

#include "ECS/Log.h"

namespace ECS {


	Domain::Domain(){}
	Domain::~Domain(){}

	Entity* Domain::createEntity() {
		auto entity = new Entity(*this);
		entities.insert(entity);
		return entity;
	}



	void Domain::flush() {
		// Delete entity components
		for( auto& entity : entitiesToDelete ) {
			delete entity;
		}
	}


	void Domain::destroyEntity(Entity* entity) {
		if( entitiesToDelete.find(entity) != entitiesToDelete.end() )
			entitiesToDelete.insert(entity);
	}


	
}