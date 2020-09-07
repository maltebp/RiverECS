#pragma once

#include <unordered_set>




namespace ECS {

	struct Entity;

	class Domain {
		std::unordered_set<Entity*> entities;
		std::unordered_set<Entity*> entitiesToDelete;


	public:

		Domain();
		~Domain();


		Entity* createEntity();




		template <typename C>
		void addEntityComponent(Entity* entity) {

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
		void flush();

	};

}






