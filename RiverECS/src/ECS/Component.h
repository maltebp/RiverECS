#pragma once

#include <string>


namespace ECS {

	using ComponentTypeId = std::string;

	// Represents a component
	using ComponentId = uint32_t;

	/**
	 *	@brief The maximum value for ComponentId
	 */
	const ComponentId MAX_COMPONENT_ID = std::numeric_limits<ComponentId>::max();

	/**
	 * @brief	All user-defined components must inherit from this struct
	*/
	struct Component {
		/**
		 * @brief	Unique ID for each component in the same type
		*/
		ComponentId id;
	};

}


