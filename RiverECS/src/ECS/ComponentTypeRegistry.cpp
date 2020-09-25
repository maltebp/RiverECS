
#include "ComponentTypeRegistry.h"

namespace ECS {

	ComponentTypeId ComponentTypeRegistry::numTypes = 0;

	std::unordered_map<std::string, ComponentTypeId> ComponentTypeRegistry::types;

}