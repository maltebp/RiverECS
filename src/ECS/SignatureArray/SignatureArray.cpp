#pragma once

#include <vector>
#include <functional>
#include <sstream>

#include "SignatureArray.h"


namespace River::ECS {


	SignatureArray::SignatureArray(unsigned int memoryStepSize) : memoryStepSize(memoryStepSize) {
		reserveMemory(memoryStepSize);
	}


	SignatureArray::~SignatureArray() {
		if( data != nullptr )
			free(data);
	}


	/**
		* @brief Removes all signatures from the array
		*
		* This deallocates the dynamic memory allocated for this list, and sets the number of
		* contained signatures to 0.
		* The signature size is not altered.
	*/
	void SignatureArray::clear() {
		free(data);
		data = nullptr;
		numSignatures = 0;
		memorySize = 0;
	}


	void SignatureArray::forMatchingSignatures(Signature& signature, std::function<void(unsigned int signatureIndex)> callback) {
		/*	There is no reason to check parts if they are all 0's, so we skip ahead and cut off the end */
		unsigned int firstPartIndex = signature.getFirstSetBit() < 0 ? 0 : signature.getFirstSetBit() / 8;
		unsigned int lastPartIndex = signature.getLastSetBit() < 0 ? 0 : signature.getLastSetBit() / 8;

		if( (signatureParts - 1) < lastPartIndex )
			/* The signatures in this array doesn't have enough parts to
				be matched with the given query, so none will match */
			return;


		// The queried signature's data
		unsigned char* signatureBits = signature.getBits();

		// For each stored signature in array
		for( unsigned int signatureIndex = 0; signatureIndex < numSignatures; signatureIndex++ ) {
			bool match = true;

			// For each part in the signature to check for match
			for( unsigned int partIndex = firstPartIndex; partIndex < lastPartIndex + 1; partIndex++ ) {
				unsigned matchingPart = data[signatureIndex * signatureParts + partIndex];

				// Check if the bits set in the queried signature are also set in the matching
				// parth, using the bitwise &.
				// Note: It doesn't have to be an exact match; the matching part may container
				// other set bits, but these are ignored
				if( (matchingPart & signatureBits[partIndex]) != signatureBits[partIndex] ) {
					match = false;
					break;
				}
			}

			if( match ) callback(signatureIndex);
		}
	}


	void SignatureArray::setSignatureBit(unsigned int signatureIndex, unsigned int bitIndex) {
		if( !(signatureIndex < numSignatures) )
			throw new IndexOutOfBoundsException(signatureIndex, numSignatures);

		// TODO: Consider adding a bit out of bounds

		bitManipulator.set(signatureIndex*signatureParts*8 + bitIndex);
	}


	void SignatureArray::unsetSignatureBit(unsigned int signatureIndex, unsigned int bitIndex) {
		if( !(signatureIndex < numSignatures) )
			throw new IndexOutOfBoundsException(signatureIndex, numSignatures);

		// TODO: Consider adding a bit out of bounds

		bitManipulator.unset(signatureIndex * signatureParts * 8 + bitIndex);
	}


	bool SignatureArray::getSignatureBit(unsigned int signatureIndex, unsigned int bitIndex) {
		if( !(signatureIndex < numSignatures) )
			throw new IndexOutOfBoundsException(signatureIndex, numSignatures);

		// TODO: Consider adding a bit out of bounds

		return bitManipulator.get(signatureIndex*signatureParts*8 + bitIndex);
	}



	void SignatureArray::setSignatureSize(unsigned int newSignatureSize) {
		if( newSignatureSize == signatureSize )
			return;

		if( newSignatureSize < signatureSize )
			throw new SignatureSizeReducedException(signatureSize, newSignatureSize);
		
		// Check if more parts are required (i.e. we go from 8 to 9 in size)
		unsigned int newSignatureParts = 1 + (newSignatureSize - 1) / 8;
		unsigned int partsDifference = newSignatureParts - signatureParts;

		if( partsDifference > 0 ) {

			// Extend memory with memory for new parts
			reserveMemory(newSignatureParts * numSignatures + memoryStepSize);

			// Pointers to last part of last signature in version of the data
			unsigned char* oldData = data + (signatureParts-1) + (numSignatures-1) * signatureParts;
			unsigned char* newData = data + (newSignatureParts-1) + (numSignatures-1) * newSignatureParts;

			// Create new empty parts in the each signatures new place place, and move old parts
			// from signatures original place
			for( int signatureIndex = numSignatures - 1; signatureIndex >= 0; signatureIndex-- ) {
				int partIndex = newSignatureParts - 1;

				// Add new empty parts
				for( unsigned int i = 0; i < partsDifference; i++ ) {
					*newData = 0;
					newData--;
					partIndex--;
				}

				// Add old existing parts
				for( unsigned int i = 0; i < signatureParts; i++ ) {
					*newData = *oldData;
					newData--;
					oldData--;
				}
			}

			signatureParts = newSignatureParts;
		}

		signatureSize = newSignatureSize;
		bitManipulator.setData(data, numSignatures * signatureParts * 8);
	}



	unsigned int SignatureArray::add() {
		reserveSignatures(++numSignatures);

		// Initialize new signature to 0
		unsigned int signatureIndex = numSignatures - 1;
		for( unsigned int i = signatureIndex * signatureParts; i < numSignatures * signatureParts; i++ )
			data[i] = 0;

		bitManipulator.setData(data, numSignatures * signatureParts * 8);

		return signatureIndex;
	}



	unsigned int SignatureArray::remove(unsigned int index) {
		if( index >= numSignatures )
			throw new IndexOutOfBoundsException(index, numSignatures);

		numSignatures--;
		if( numSignatures == 0 || numSignatures == index ) return 0;
		
		// Move last signature to removed signatures slot
		unsigned char* lastSignature = data + (size_t)numSignatures * signatureParts;
		unsigned char* deletedSignature = data + (size_t)index * signatureParts;
		for( unsigned int part = 0; part < signatureParts; part++ )
			deletedSignature[part] = lastSignature[part];

		bitManipulator.setData(data, numSignatures * signatureParts*8);

		return numSignatures;
	}

	
	void SignatureArray::reserveSignatures(unsigned int newNumSignatures) {
		unsigned int newMemorySize = memorySize;
		while( newMemorySize/signatureParts < newNumSignatures ) {
			newMemorySize += memoryStepSize;
		}
		
		if( newMemorySize > memorySize )
			reserveMemory(newNumSignatures * signatureParts);
	}


	void SignatureArray::reserveMemory(unsigned int bytes) {
		if( bytes <= memorySize ) return;

		auto newData = (unsigned char*) realloc(data, sizeof(unsigned char) * bytes);
		if( newData == nullptr )
			throw MemoryAllocationException(bytes);
		
		data = newData;
		memorySize = bytes;

		bitManipulator.setData(data, numSignatures*signatureParts*8);
	}


	unsigned int SignatureArray::getNumSignatures() {
		return numSignatures;
	}

	unsigned int SignatureArray::getMemorySize() {
		return memorySize;
	}

}