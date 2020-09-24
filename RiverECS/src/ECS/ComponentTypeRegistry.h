#pragma once

#include <unordered_map>
#include <typeinfo>

#include "Component.h"


namespace ECS {
	class ComponentTypeRegistry {
	public:


		/*template <typename C>
		static bool isTypeRegistered() {
			RV_ECS_ASSERT_COMPONENT_TYPE(C);
			return types.find(typeid(C).name()) != types.end();
		}*/


		template <typename C>
		static ComponentTypeId getTypeId() {
			RV_ECS_ASSERT_COMPONENT_TYPE(C);
			
			auto typeName = std::string(typeid(C).name());
			auto typeIdIterator = types.find(typeName);
			if( typeIdIterator != types.end() ) return typeIdIterator->second;

			auto typeId = types.emplace(typeName, numTypes++);

			return typeId.second;
		}


		static unsigned int getNumTypes() {
			return types.size();
		}


	private:
		static ComponentTypeId numTypes;
		static std::unordered_map<std::string, ComponentTypeId> types;

	};


	/*unsigned int ComponentTypeRegistry::numTypes = 0;
	std::unordered_map<std::string, ComponentTypeId> ComponentTypeRegistry::types;*/
}


