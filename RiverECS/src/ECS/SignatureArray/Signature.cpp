#include "Signature.h"

namespace ECS {


	Signature::Signature(unsigned int size) :
		BitManipulator((unsigned char*) calloc( (1+(size-1)/8), 1), size)
	{}

	Signature::~Signature() {
		free(bits);
	}


	Signature& Signature::operator=(const Signature& other)	{
		free(bits);
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
		unsigned firstNewByte = parts;
		for( int i = parts; i < newParts; i++ ) {
			bits[i] = 0;
		}

		parts = newParts;
	}

}
