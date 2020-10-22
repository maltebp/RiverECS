
#include <catch.h>
#include <stdlib.h>

#include <ECS.h>

#include "Log.h"
#include "TestComponents.h"
#include <numeric>





//struct Entity {
//	River::ECS::Entity* ptr;
//	Entity(River::ECS::Domain& domain) {
//		ptr = domain.createEntity();
//	}
//
//	virtual const std::string getType() = 0;
//};
//
//
//struct EntityHandler {
//
//	std::unordered_set<Entity*> allEntities;
//	std::unordered_map<const std::string, std::unordered_set<Entity*>> entityLists;
//
//
//	template <typename T>
//	void createEntity(River::ECS::Domain& domain) {
//		Entity* entity = new T(domain);
//		allEntities.insert(entity);
//		entityLists[entity->getType()].insert(entity);
//	}
//
//	template <typename T>
//	void deleteEntity(Entity* entity) {
//		allEntities.erase(entity);
//		entityLists[entity->getType()].erase(entity);
//	}
//
//
//	unsigned int getTotalEntityCount() {
//		return allEntities.size();
//	}
//
//	
//	template <typename T>
//	unsigned int getEntityTypeCount() {
//		return entityLists[typeid(T).name()].size();
//	}
//};
//
//
//
//
//struct Entity1 : public Entity {
//	Entity1(River::ECS::Domain& domain) : Entity(domain) {
//		ptr->addComponent<ComponentA>();
//		ptr->addComponent<ComponentB>();
//		ptr->addComponent<ComponentC>();
//		ptr->addComponent<ComponentD>();
//	}
//
//	const std::string getType() override {
//		return typeid(Entity1).name();
//	}
//};
//
//
//struct Entity2 : public Entity {
//	Entity2(River::ECS::Domain& domain) : Entity(domain) {
//		ptr->addComponent<ComponentA>();
//		ptr->addComponent<ComponentB>();
//		ptr->addComponent<ComponentC>();
//		ptr->addComponent<ComponentD>();
//	}
//
//	const std::string getType() override {
//		return typeid(Entity1).name();
//	}
//};
//
//
//struct Entity3 : public Entity {
//	Entity3(River::ECS::Domain& domain) : Entity(domain) {
//		ptr->addComponent<ComponentA>();
//		ptr->addComponent<ComponentB>();
//		ptr->addComponent<ComponentC>();
//		ptr->addComponent<ComponentD>();
//	}
//
//	const std::string getType() override {
//		return typeid(Entity1).name();
//	}
//};
//
//
//struct Entity4 : public Entity {
//	Entity4(River::ECS::Domain& domain) : Entity(domain) {
//		ptr->addComponent<ComponentA>();
//		ptr->addComponent<ComponentB>();
//		ptr->addComponent<ComponentC>();
//		ptr->addComponent<ComponentD>();
//	}
//
//	const std::string getType() override {
//		return typeid(Entity1).name();
//	}
//};
//
//
//struct Entity5 : public Entity {
//	Entity5(River::ECS::Domain& domain) : Entity(domain) {
//		ptr->addComponent<ComponentA>();
//		ptr->addComponent<ComponentB>();
//		ptr->addComponent<ComponentC>();
//		ptr->addComponent<ComponentD>();
//	}
//
//	const std::string getType() override {
//		return typeid(Entity1).name();
//	}
//};



/**
 * @brief	Utility function to get vector of type names
*/
template<typename ... T>
std::vector<std::string> getTypeIds() {
	std::vector<std::string> types;
	(types.push_back(typeid(T).name()), ...); // C++17 fold expression
	return types;
}




struct IComponentType {
	virtual void addToEntity(River::ECS::Entity*) = 0;
	virtual void removeFromEntity(River::ECS::Entity*) = 0;
	virtual std::string getComponentId() = 0;
};


template<typename Component>
struct ComponentType : IComponentType{
	virtual void addToEntity(River::ECS::Entity* entity) override {
		entity->addComponent<Component>();
	}
	virtual void removeFromEntity(River::ECS::Entity* entity) override {
		entity->removeComponent<Component>();
	}

	virtual std::string getComponentId() override {
		return typeid(Component).name();
	}
};


struct IQuery {
	virtual unsigned int run(River::ECS::Domain& domain) = 0;
	virtual std::vector<std::string> getComponentIds() = 0;
};

template <typename ...Components>
struct Query : IQuery {

	unsigned int run(River::ECS::Domain& domain) override {
		int count = 0;
		domain.forMatchingEntities<Components... >([&](auto entity, auto... components) {
			count++;
		});
		return count;
	}


	virtual std::vector<std::string> getComponentIds() {
		return getTypeIds<Components...>();
	}

	
	
};


struct IEntityType {
	virtual River::ECS::Entity* create(River::ECS::Domain& domain) = 0;
	virtual std::vector<std::string> getComponentIds() = 0;
};





struct TestController {


	void testQueries() {
		domain.clean();

		for( auto& query : queries ) {
			unsigned int queryCount = query->run(domain);
			unsigned int entityCount = getEntityCount(query->getComponentIds());
			REQUIRE(entityCount == queryCount);
		}
	}

	unsigned int getEntityCount(const std::vector<std::string>& componentIds) {
		unsigned int count = 0;
		for( auto& pair : entityComponents ) {
			auto entityComponents = pair.second;
			bool match = true;
			for( auto componentId : componentIds ) {
				if( entityComponents.find(componentId) == entityComponents.end() ) {
					match = false;
					break;
				}
			}
			if( match )
				count++;
		}
		return count;
	}


	void createRandomEntity(unsigned int numComponents) {
		auto entity = domain.createEntity();
		
		// Create copy
		std::vector<IComponentType*> componentTypes(this->componentTypes);

		auto& componentSet = entityComponents[entity];

		for( int i = 0; i < numComponents; i++ ) {
			auto iterator = componentTypes.begin() + (rand() % componentTypes.size());
			(*iterator)->addToEntity(entity);
			componentSet.insert((*iterator)->getComponentId());
			componentTypes.erase(iterator);
		}

		entities.push_back(entity);
	}


	void deleteRandomEntity() {
		auto listIterator = entities.begin() + (rand() % componentTypes.size());
		auto entity = *listIterator;
		entities.erase(listIterator);
		
		entityComponents.erase(entity);
		
		domain.destroyEntity(entity);
	}

	// TODO: Implement this in test
	void deleteRandomEntityComponent() {

	}

	unsigned int getTotalEntityCount() {
		return entities.size();
	}

	template<typename ...Q>
	void addQueries(Q ... q) {
		(queries.push_back(q), ...);
	}


	template<typename ...C>
	void addComponentTypes() {
		(componentTypes.push_back(new ComponentType<C>), ...);
	}


private:
	std::vector<River::ECS::Entity*> entities;
	std::unordered_map<River::ECS::Entity*, std::unordered_set<std::string>> entityComponents;
	std::vector<IQuery*> queries;
	std::vector<IComponentType*> componentTypes;
	River::ECS::Domain domain;
};



//template <typename ComponentTypes>
//struct DomainController {
//	River::ECS::Domain domain;
//
//
//	DomainController() {
//
//	}
//
//
//	template <C>
//	void checkComponentType() {
//
//	}
//
//
//
//	void clean() {
//		domain.clean();
//	}
//
//};


// List: components to add
// List: entities to add
// List: entities to delete
// List: components to delete



//template <typename ...Components>
//void testQuery(River::ECS::&Domain) {
//
//}


TEST_CASE("General Test", "[general]") {


	TestController testController;

	testController.addComponentTypes<
		ComponentA,
		ComponentB,
		ComponentC,
		ComponentD
	>();

	testController.addQueries(
		new Query<ComponentA>(),
		new Query<ComponentB>(),
		new Query<ComponentC, ComponentD>(),
		new Query<ComponentA, ComponentB>(),
		new Query<ComponentA, ComponentB, ComponentC, ComponentD>()
	);
	

	for( int i=0; i < 100; i++ )
		testController.createRandomEntity( 1 + (rand()%4) );


	

	testController.testQueries();


	for( int i = 0; i < 50; i++ )
		testController.createRandomEntity(1 + (rand() % 4));


	testController.testQueries();


	//River::ECS::Domain domain;


	/*auto entity = domain.createEntity();
	entity->addComponent<ComponentA>();
	entity->addComponent<ComponentB>();*/
	
	/*unsigned int seed = 123456789;
	srand(seed);*/

	//std::vector<Query*> queries;

	/*std::function<void(River::ECS::Domain& domain)> callback = [](River::ECS::Domain& domain) {
		domain.forMatchingEntities<ComponentA, ComponentB, ComponentC>() {

		}
	};*/

	/*Query* query = new QueryType<ComponentA, ComponentB>();
	query->run(domain);

	for( auto compId : query->getComponentIds() ) {
		LOG(compId);
	}*/

	//queries.push_back();

	//int count = queries.back()->run(domain);
	//LOG(count);

	// 1000000 Iterations
	//for( int i = 0; i < 100000; i++ ) {

		// Check each single signature
		// Check combination of signatures

		// Add Entities

		// Delete entities

		// Add component to existing entities

		// Delete components from entities

		// Check each single signature

		// Check each single signature
		// Check combination of signatures

	//}
	


}

