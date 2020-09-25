#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "ComponentTypeRegistry.h"
#include "ComponentController.h"
#include "Component.h"

#include "SignatureArray/Signature.h"
#include "SignatureArray/SignatureArray.h"


namespace ECS {

	struct Entity;

	class Domain {
	public:

		Domain();
		~Domain();


		Entity* createEntity();


		template <typename C>
		C* addEntityComponent(Entity* entity) {
			RV_ECS_ASSERT_COMPONENT_TYPE(C);
			auto componentController = getComponentController<C>();		
			auto component = componentController->createComponent(entity);


			entityComponentsToCreate[entity].push_back(ComponentTypeRegistry::getTypeId<C>());
			return component;
		}



		template <typename C>
		C* getEntityComponent(Entity* entity) {
			RV_ECS_ASSERT_COMPONENT_TYPE(C);
			auto componentController = getComponentController<C>();
			return componentController->getComponent(entity);
		}

		template <typename C>
		void removeEntityComponent(Entity* entity) {
			RV_ECS_ASSERT_COMPONENT_TYPE(C);
			entityComponentsToDelete[entity].push_back(ComponentTypeRegistry::getTypeId<C>());
		}







		template <typename ... C, typename Func>
		void forEachEntity(Func callback) {
			func 
		}



		/*template <typename T>
		T* getEntityComponent(Entity* entity) {

			LOG("Adding component");

		}*/


		/*template <typename T>
		T getComponent(ComponentId<T> id);*/


		/**
		 * @brief Marks Entity for deletion on next flush
		 * @param  
		*/
		void destroyEntity(Entity*);


		/**
		 * @brief Flushes the Domain, deleting all Entities marked for deletion
		*/
		void clean();



		
	private:
		template <typename C>
		ComponentController<C>* getComponentController() {
			RV_ECS_ASSERT_COMPONENT_TYPE(C);

			auto componentTypeId = ComponentTypeRegistry::getTypeId<C>();
			auto controllerIterator = componentControllers.find(componentTypeId);

			if( controllerIterator != componentControllers.end() )
				return (ComponentController<C>*) controllerIterator->second;

			auto emplaceResult = componentControllers.emplace(componentTypeId, new ComponentController<C>());
			return (ComponentController<C>*) emplaceResult.first->second;
		}


	private:
		std::vector<Entity*> entities;

		/**
		 * @brief Maps an Entity to its index into the 'entities' vector */
		std::unordered_map<Entity*, unsigned int> entityIndices;

		std::unordered_set<Entity*> newEntities;
		std::unordered_set<Entity*> entitiesToDelete;

		/**
		 * @brief Maps an entity to an index into the signature array */
		std::unordered_map<Entity*, unsigned int> entitySignatureIndexMap;

		/**
		 * @brief Maps an signature index to an entiy */
		std::unordered_map<unsigned int, Entity*> signatureIndexEntityMap;


		std::unordered_map<Entity*, std::vector<ComponentTypeId>> entityComponentsToCreate;
		std::unordered_map<Entity*, std::vector<ComponentTypeId>> entityComponentsToDelete;
		


		SignatureArray signatures;

		std::unordered_map<ComponentTypeId, IComponentController*> componentControllers;

	};

}






