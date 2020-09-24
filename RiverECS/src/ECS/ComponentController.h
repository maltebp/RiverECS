#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <limits>
#include <type_traits>

#include "Component.h"
#include "Exception.h"



namespace ECS {
	struct Entity;

	// Thrown if an Entity already has a component
	class MultipleComponentException : public Exception {
	public:
		MultipleComponentException(const std::string& componentName) : Exception("Entity already has component '" + componentName + "'") {}
	};

	// Thrown if the all compnent id has been used 
	class MaxComponentsException : public Exception {
	public:
		MaxComponentsException(const std::string& componentName) : Exception("Max number of components (" + std::to_string(MAX_COMPONENT_ID) + ") has been created for component '" + componentName + "'") {}
	};



	/**
	 * @brief	Manages the creation, storage and deletion of a specifc component type
	 * @tparam  C	The type of component (must inherit from ECS::Component)
	*/
	template <typename C>
	class ComponentController {
		static_assert(std::is_base_of<Component, C>::value, "Component types must inherit from ECS::Component");

		const static unsigned int SECONDARY_LIST_SIZE = 20;

	public:
		ComponentController() {}


		/**
		 * @brief	Creates a new component for the given Entity
		 * 
		 * @return	A temporary pointer to the component. The pointer is invalidated after the compress() or clean() is called
		*/
		C* createComponent(Entity* entity) {
			/*	Created components are stored in the primary list only if the
				primary list already has memory allocated for it. Otherwise, it's
				stored in one of the secondary lists until the the controllers
				'compress' method is called. This is to prevent invalidation of 
				pointers to elements in the primary list, in case new components
				are created during iteration of primary components.
			*/

			if( componentMap.find(entity) != componentMap.end() )
				throw MultipleComponentException(typeid(C).name()); // Throw exception

			// Register Entity's component
			ComponentId id = createComponentId();
			indexMap[id] = numComponents;
			entityMap[id] = entity;
			componentMap[entity] = id;

			C& component = getNewComponent(numComponents);
			component.id = id;

			numComponents++;

			return &component;
		}


		/**
		 * @brief	Marks the component for deletion for the given Entity. The components will keep existing,
					until the Controller is cleaned by calling clean().
					Does nothing if the Entity doesn't the particular component.
		*/
		void deleteComponent(Entity* entity) {
			auto iterator = componentMap.find(entity);
			if( iterator == componentMap.end() ) return;
			componentsToDelete.insert(iterator->second);
		}


		/**
		 * @brief	Get a pointer to the Entity's component
		 * @return	Temporary to pointer the Entity's component, or nullptr if the Entity doesn't have this component.
					The pointer is invalidated when the Controller is cleaned or compressed.
		*/
		C* getComponent(Entity* entity) {
			auto iterator = componentMap.find(entity);
			if( iterator == componentMap.end() )
				return nullptr;

			return getComponent(iterator->second);
		}


		/**
		 * @brief	Get a pointer to the Component with the given ComponentId
		 * @param id	The ID of the component to retrieve
		 * @return	Temporary to pointer Component, or nullptr if the the 'id' is not in use.
					The pointer is invalidated when the Controller is cleaned or compressed.
		*/
		C* getComponent(ComponentId id) {
			// Check if id exists
			auto iterator = indexMap.find(id);
			if( iterator == indexMap.end() )
				return nullptr;

			// Find component primary list
			unsigned int index = iterator->second;
			unsigned int primaryListSize = (unsigned int) primaryList.size();
			if( index < primaryListSize )
				return &primaryList.at(index);

			// Find component in secondary list
			unsigned int adjustedIndex = (index - primaryListSize);
			unsigned int listIndex = adjustedIndex / SECONDARY_LIST_SIZE;
			unsigned int elementIndex = adjustedIndex % SECONDARY_LIST_SIZE;

			return &secondaryLists.at(listIndex).at(elementIndex);
		}

		
		/**
		 * @brief	Deletes components that are marked for deletion, and compress the data structure.
		 *			This invalidated all pointers to Components returned by the controller
		*/
		void clean() {
			compress();

			// we assume that the lists have been compressed when deleting
			for( auto& componentId : componentsToDelete ) {
				auto entity = entityMap.find(componentId)->second;
				unsigned int index = indexMap.find(componentId)->second;

				auto& component = primaryList.at(index);

				// Move last component
				if( (index+1) < numComponents ) {
					auto& last = primaryList.at(index);	
					component = last;
					indexMap.at(last.id) = index;
				}

				// Delete component
				componentMap.erase(entity);
				entityMap.erase(componentId);
				indexMap.erase(componentId);

				numComponents--;
			}
		}


		/**
		 * @brief	Compresses the primary and secondary list into a single consecutive memory block.
		 *			This invalidated all pointers to Components returned by the controller
		*/
		void compress() {
			auto numSecondaryLists = secondaryLists.size();
			if( numSecondaryLists == 0 ) return;

			unsigned int primaryListSize = (unsigned int) primaryList.size();
			unsigned int insertIndex = primaryListSize;
			unsigned int remainingComponents = numComponents - primaryListSize;

			primaryList.resize(numComponents);
			
			for( auto& secondaryList : secondaryLists ) {
				for( auto& component : secondaryList ) {
					if( insertIndex >= numComponents ) break;
					auto& newDestination = primaryList.at(insertIndex);
					newDestination = component;
					insertIndex++;
				}
			}

			secondaryLists.clear();
		}


		/*
		 * @return	Current number of components
		*/
		unsigned int getNumComponents() {
			return numComponents;
		}


	private:
		// Component controller should never be moved
		ComponentController(ComponentController&& other) = delete;
		ComponentController(const ComponentController&) = delete;
		ComponentController& operator=(const ComponentController&) = delete;

		/**
		 * @brief	Finds the next available component id
		*/
		ComponentId createComponentId() {
			if( numComponents >= MAX_COMPONENT_ID )
				throw new MaxComponentsException(typeid(C).name());

			while( indexMap.find(nextId) != indexMap.end() ) {
				if( nextId == MAX_COMPONENT_ID )
					nextId = 0;
				else
					nextId++;
			}
			return nextId;
		}


		/**
		 * @brief	Finds the component for the given index. This component may reside in the primaray list
					or one of the secondary lists. If the index goes beyond current number of secondary lists
					then a new secondary list is created.

		 * @param	index	The index of the component (typically its number-1)
		 * @return	Reference to the component 
		*/
		C& getNewComponent(unsigned int index) {
			// Check if resides in primary list
			unsigned int primaryListSize = (unsigned int) primaryList.size();
			if( index < primaryListSize )
				return primaryList.at(index);

			// FInd place in secondary list
			unsigned int adjustedIndex = (index - primaryListSize);
			unsigned int listIndex = adjustedIndex / SECONDARY_LIST_SIZE;
			unsigned int elementIndex = adjustedIndex % SECONDARY_LIST_SIZE;

			if( listIndex == secondaryLists.size() ) {
				// All lists are full - create new secondary list
				secondaryLists.emplace_back();
				secondaryLists.back().resize(SECONDARY_LIST_SIZE);
			}
			return secondaryLists.at(listIndex).at(elementIndex);
		}


	private:
		ComponentId nextId = 0;
		
		// Maps entity to component id
		std::unordered_map<Entity*, ComponentId> componentMap;

		// Maps component id to entity
		std::unordered_map<ComponentId, Entity*> entityMap;

		// Maps component id to index in component list
		std::unordered_map<ComponentId, unsigned int> indexMap;

		std::unordered_set<ComponentId> componentsToDelete;

		unsigned int numComponents = 0;
		std::vector<C> primaryList;

		// Temporary storage for components created in between compress() calls
		std::vector<std::vector<C>> secondaryLists;
	};



}
