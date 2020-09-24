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
			newEntityComponents.emplace(entity, ComponentTypeRegistry::getTypeId<C>());
			return component;
		}



		template <typename C>
		C* getEntityComponent(Entity* entity) {
			auto componentController = getComponentController<C>();
			return componentController->getComponent(entity);
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


		template <typename C>
		ComponentController<C>* getComponentController() {
			RV_ECS_ASSERT_COMPONENT_TYPE(C);

			auto componentTypeId = ComponentTypeRegistry::getTypeId<C>();
			auto controllerIterator = componentControllers.find(componentTypeId);

			if( controllerIterator != componentControllers.end() )
				return (ComponentController<C>*) controllerIterator->second;

			return (ComponentController<C>*) componentControllers.emplace(componentTypeId, new ComponentController<C>()).second;
		}


		
	private:
		std::vector<Entity*> entities;

	private:
		/**
		 * @brief Maps an Entity to its index into the 'entities' vector */
		std::unordered_map<Entity*, unsigned int> entityIndices;

		std::unordered_set<Entity*> newEntities;
		std::unordered_set<Entity*> entitiesToDelete;


		std::unordered_map<Entity*, ComponentTypeId> newEntityComponents;


		SignatureArray signatures;
		std::unordered_map<ComponentTypeId, IComponentController*> componentControllers;

	};

}






