#include "Domain.h"

#include "Entity.h"

#include "ECS/Log.h"

namespace ECS {

	/**
	 * @brief 
	*/
	Domain::Domain() : signatures(5000) {
	
	}


	Domain::~Domain(){
		for( auto componentController : componentControllers )
			delete componentController.second;
		for( auto entity : entities )
			delete entity;	
	}


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
		signatures.reserveSignatures(signatures.getNumSignatures() + newEntities.size());
		entities.reserve(entities.size() + newEntities.size());
		for( auto& newEntity : newEntities ) {
			entities.push_back(newEntity);
			// Adding signature (not registering components)
			auto signatureIndex = signatures.add();
			entitySignatureIndexMap.emplace(newEntity, signatureIndex);
			signatureIndexEntityMap.emplace(signatureIndex, newEntity);
		}

		// Moving new components into signatures
		for( auto& pair : entityComponentsToCreate ) {
			auto entity = pair.first;
			auto signatureIndex = entitySignatureIndexMap.find(entity)->second;
			for( auto& component : pair.second ) {
				signatures.setSignatureBit(signatureIndex, component);
			}
		}

		// Delete entity components
		for( auto& pair : entityComponentsToDelete ) {
			auto entity = pair.first;
			auto signatureIndex = entitySignatureIndexMap.find(entity)->second;
			for( auto& component : pair.second ) {
				signatures.unsetSignatureBit(signatureIndex, component);
				componentControllers.find(component)->second->deleteComponent(entity);
			}
		}

		// Delete entities
		for( auto& entity : entitiesToDelete ) {
			auto signatureIndex = entitySignatureIndexMap.find(entity)->second;
			
			auto movedSignature = signatures.remove(signatureIndex);
			if( movedSignature != 0 ) {
				// Change signature index of moved signature (signature which was moved into the deleted signature's place)
				auto movedEntity = signatureIndexEntityMap.find(movedSignature)->second;
				signatureIndexEntityMap.erase(movedSignature);
				signatureIndexEntityMap.find(signatureIndex)->second = movedEntity;
				entitySignatureIndexMap.find(movedEntity)->second = movedSignature;
			}

			entitySignatureIndexMap.erase(entity);

			// TODO: Use signatures.forEachBit thingy to iterate over ComponentControllers
			for( auto& componentController : componentControllers ) {
				componentController.second->deleteComponent(entity);
			}

			
			// Remove entity from entities list
			auto entityIterator = std::find(entities.begin(), entities.end(), entity);
			if( entityIterator != entities.end() )
				entities.erase(entityIterator);

			delete entity;
		}

		// Clean component controllers (delete components marked for deletion)
		for( auto& componentController : componentControllers ) {
			componentController.second->clean();
		}


		entityComponentsToCreate.clear();
		entityComponentsToDelete.clear();
		entitiesToDelete.clear();
		newEntities.clear();
	}


	void Domain::destroyEntity(Entity* entity) {
		/* This has to be implemented in the .cpp file, due to cyclic includes */
		if( entitiesToDelete.find(entity) == entitiesToDelete.end() )
			entitiesToDelete.insert(entity);
	}




	
}