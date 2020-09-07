#pragma once

#include <cstdint>
#include "Domain.h"

namespace ECS {

	struct Entity {
		Domain& domain;

		template <typename C>
		void addComponent() {
			domain.addEntityComponent<C>(this);
		}

		void destroy();



	private:
		Entity(Domain& domain) : domain(domain) {}

		// Prevent entity from being deleted by anyone else than Domain
		~Entity() {}

		Entity(const Entity& other) = delete;
		// TODO: Delete move contructor, and = operator constructors as well


		friend class Domain;
	};



}