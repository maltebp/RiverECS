#include "Signature.h"

namespace River::ECS {


	Signature::Signature(unsigned int size) :
		BitManipulator((unsigned char*) calloc( (1+(size-1)/8), 1), size)
	{}

	Signature::~Signature() {
		free(bits);
	}


	Signature& Signature::operator=(const Signature& other)	{
		free(bits);
		firstBit = other.firstBit;
		lastBit = other.lastBit;
		bitsSet = other.bitsSet;
		dirty = other.dirty;
		size = other.size;
		parts = other.parts;
		bits = (unsigned char*)calloc(parts, 1);
		for( unsigned int i = 0; i < parts; i++ )
			bits[i] = other.bits[i];
		return *this;
	}


	Signature::Signature(const Signature& other) :
		BitManipulator((unsigned char*) calloc(other.parts, 1), other.size)
	{
		firstBit = other.firstBit;
		lastBit = other.lastBit;
		bitsSet = other.bitsSet;
		dirty = other.dirty;

		for( unsigned int i = 0; i < parts; i++ )
			bits[i] = other.bits[i];
	}



	void Signature::resize(unsigned int newSize) {
		if( newSize == size ) return;
		if( newSize < size ) throw new SignatureSizeReducedException(size, newSize);

		size = newSize;

		// Check new memory size
		unsigned int newParts = 1 + (newSize - 1) / 8;
		if( newParts == parts ) return;

		// Reallocate memory and initialize data to 0
		bits = (unsigned char*) realloc(bits, newParts);
		unsigned int firstNewByte = parts;
		for( unsigned int i = parts; i < newParts; i++ ) {
			bits[i] = 0;
		}

		parts = newParts;
	}

}
