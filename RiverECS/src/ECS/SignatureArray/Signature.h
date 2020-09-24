#pragma once

#include "BitManipulator.h"


namespace ECS {


	/**
	 * @brief Data is heap allocated, so it's not efficient to copy this
	*/
	class Signature : public BitManipulator {
	public:

		Signature(unsigned int size);
		~Signature();

		Signature& operator=(const Signature& other);
		Signature(const Signature& other);
	};
}



