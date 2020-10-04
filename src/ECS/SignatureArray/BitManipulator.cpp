#include "BitManipulator.h"


namespace River::ECS {

	BitManipulator::BitManipulator(unsigned char* data, unsigned int numBits) {
		setData(data, numBits);
	}


	void BitManipulator::setData(unsigned char* data, unsigned int numBits) {
		bits = data;
		size = numBits;
		parts = 1 + (numBits - 1) / 8;
		dirty = true;
	}


	bool BitManipulator::get(unsigned int i) const {
		if( !(i < size) )
			throw std::out_of_range(("Bit index out of range (i=" + std::to_string(i) + ", size=" + std::to_string(size) + ")").c_str());

		unsigned int partIndex = i / 8;
		unsigned int bit = i % 8;

		unsigned char mask = 1 << bit;
		unsigned char& part = *(bits + partIndex);
		return part & 1 << bit;
	}


	void BitManipulator::set(unsigned int i) {
		if( !(i < size) )
			throw std::out_of_range(("Bit index out of range (i=" + std::to_string(i) + ", size=" + std::to_string(size) + ")").c_str());

		unsigned int partIndex = i / 8;
		unsigned int bit = i % 8;

		unsigned char& part = *(bits + partIndex);
		part = part | ((unsigned char)1) << bit;
		dirty = true;
	}


	void BitManipulator::unset(unsigned int i) {
		if( !(i < size) )
			throw std::out_of_range(("Bit index out of range (i=" + std::to_string(i) + ", size=" + std::to_string(size) + ")").c_str());

		unsigned int partIndex = i / 8;
		unsigned int bit = i % 8;

		unsigned char& part = *(bits + partIndex);

		part = part & ~(((unsigned char)1) << bit);
		dirty = true;
	}

	void BitManipulator::unsetAll() {
		for( unsigned int i = 0; i < parts; i++ )
			bits[i] = 0;
		firstBit = -1;
		lastBit = -1;
		dirty = false;
	}

	int BitManipulator::getFirstSetBit() {
		if( dirty ) checkBits();
		return firstBit;
	}

	int BitManipulator::getLastSetBit() {
		if( dirty ) checkBits();
		return lastBit;
	}

	unsigned int BitManipulator::getBitsSet() {
		if( dirty ) checkBits();
		return bitsSet;
	}

	unsigned char* BitManipulator::getBits() {
		return bits;
	}

	unsigned int BitManipulator::getSize() {
		return size;
	}

	
	void BitManipulator::forEachSetBit(std::function<void(unsigned int)> callback) {
		if( dirty ) checkBits();

		// No need to iterate if no bits are set
		if( bitsSet == 0 ) return;

		// Limit the iteration interval to parts with bits set
		unsigned int firstPart = firstBit / 8;
		unsigned int lastPart = lastBit / 8;

		// Iterate over each part (byte / unsigned char)
		for( unsigned int i = firstPart; i <= lastPart; i++ ) {
			unsigned char part = bits[i];
			if( part == 0 ) continue; // No bits are set, so no need to check

			unsigned char mask = 1; // Determines which bit we are looking at 
			for( int j = 0; j < 8; j++ ) {
				// Check if bit is set
				if( (part & mask) ) {
					int bitIndex = i * 8 + j;
					callback(bitIndex ); 
				}
				mask = mask << 1;
			}
		}
	}

	/**
	 * @brief	Finds the first bit, last bit and the number of bits set in this signature
	*/
	void BitManipulator::checkBits() {
		firstBit = -1;
		lastBit = -1;
		bitsSet = 0;

		// Iterate over each part (byte / unsigned char)
		for( unsigned int i = 0; i < parts; i++ ) {
			unsigned char part = bits[i];
			if( part == 0 ) continue; // No bits are set, so no need to check

			unsigned char mask = 1; // Determines which bit we are looking at 
			for( int j = 0; j < 8; j++ ) {
				// Check if bit is set
				if( (part & mask) ) {
					int bitIndex = i * 8 + j;
					if( firstBit < 0 ) firstBit = bitIndex;
					lastBit = bitIndex;
					bitsSet++;
				}
				mask = mask << 1;
			}
		}
		dirty = false;
	}

}