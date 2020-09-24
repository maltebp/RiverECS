#include "Domain.h"

#include "Entity.h"

#include "ECS/Log.h"

namespace ECS {

	/**
	 * @brief 

	*/
	Domain::Domain() : signatures(5000) {}
	Domain::~Domain(){}


	Entity* Domain::createEntity() {
		/* This has to be implemented in the .cpp file, due to cyclic includes */
		auto entity = new Entity(*this);
		newEntities.emplace(entity);		
		return entity;
	}

	void Domain::clean() {

		// Adjust size of signaturearray
		signatures.setSignatureSize(ComponentTypeRegistry::getNumTypes());
		
		// Move new entities
		entities.reserve(entities.size() + newEntities.size());
		for( auto& newEntity : newEntities ) {
			entities.push_back(newEntity);
		}



		// Delete entity components
		for( auto& entity : entitiesToDelete ) {
			delete entity;
		}


	}


	void Domain::destroyEntity(Entity* entity) {
		/* This has to be implemented in the .cpp file, due to cyclic includes */
		if( entitiesToDelete.find(entity) != entitiesToDelete.end() )
			entitiesToDelete.insert(entity);
	}




	
}