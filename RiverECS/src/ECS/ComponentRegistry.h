#pragma once

#include "Component.h"


namespace ECS {
	class ComponentRegistry {
	public:

		template <typename C>
		static ComponentTypeId registerComponent();

		template <typename C>
		static bool isComponentRegistered();

		static unsigned int getNumComponents();





	private:
		void getRegisteredComponents


	};
}


