#pragma once

#include <string>

#include "ECS/Exception.h"

namespace ECS {

	/**
	 * @brief	Wrapper class for setting and unsetting bits in an array of unsigned characters
	 *
	 * @details The class doesn't create the data, but is being passed the data from the creator
				of the object.
	*/
	class BitManipulator {
	public:

		/**
		 * @brief	Construct from an existing array of unsigned characters. It's the creator's
					responsibility to update the data pointer in case it's moved.

					WARNING: If the data is updated from somewhere else, the manipulator may become corrupted

		 * @param	data	Pointer to array of data
		 * @param	size	Number of bits to manipulate in the data
		*/
		BitManipulator(unsigned char* data, unsigned int numBits);

		/**
		 * @return	Whether or not the i'th bit is set 
		*/
		bool get(unsigned int i) const;

		/**
		 * @brief	Sets the i'th (sets value to 1)
		*/
		void set(unsigned int i);

		/**
		 * @brief	Unsets the i'th (sets value to 0)
		*/
		void unset(unsigned int i);

		/**
		 * @brief	Unsets all bits (sets all values to 0)
		*/
		void unsetAll();

		/**
		 * @return	The index of the first bit which is set, or -1 if no bits are set
		*/
		int getFirstSetBit();

		/**
		 * @return	The index of the last bit which is set, or -1 if no bits are set
		*/
		int getLastSetBit();

		/**
		 * @return	The number of bits which are set
		*/
		unsigned int getBitsSet();

		/**
		 * @return	The data (bits) which the manipulator is operation on
		*/
		unsigned char* getBits();


	private:

		/**
		 * @brief	Updates which bit is the last and first set (firstBit and lastBit), as well as the number of bits set (bitsSet)
					This sets the 'dirty' flag to false.
		 *
		 * @details Since this function is relatively slow, it's called only when either of the variables are requested, and not when
					the values wrong.
		*/
		void checkBits();


	protected:
		unsigned char* bits;
		unsigned int size;
		unsigned int parts;

		bool dirty = false;
		unsigned int bitsSet = 0;
		int firstBit = -1;
		int lastBit = -1;
	};

}
