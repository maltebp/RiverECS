
#include "ComponentTypeRegistry.h"

namespace ECS {

	ComponentTypeId ComponentTypeRegistry::numTypes;

	std::unordered_map<std::string, ComponentTypeId> ComponentTypeRegistry::types;

}