#pragma once

#include <vector>
#include <functional>
#include <sstream>

#include "SignatureArray.h"


namespace ECS {


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
	}


	/**
		* @brief Checks if each signature in the array matches the given signature, and call the callback function in case it does
		* @param signature
		* @param callback
	*/
	void SignatureArray::forMatchingSignatures(Signature& signature, std::function<void(unsigned int signatureIndex)> callback) {

		/*	There is no reason to check parts if they are all 0's, so we skip ahead */
		unsigned int firstPartIndex = signature.getFirstSetBit() < 0 ? 0 : signature.getFirstSetBit() / 8;
		unsigned int lastPartIndex = signature.getLastSetBit() < 0 ? 0 : signature.getLastSetBit() / 8;

		if( (signatureParts - 1) < lastPartIndex )
			return;// TODO: Throw exception: they don't match

		unsigned char* signatureBits = signature.getBits();

		for( unsigned int signatureIndex = 0; signatureIndex < numSignatures; signatureIndex++ ) {
			bool match = true;

			for( unsigned int partIndex = firstPartIndex; partIndex < lastPartIndex + 1; partIndex++ ) {
				unsigned matchingSignature = data[signatureIndex * signatureParts + partIndex];
				if( (matchingSignature & signatureBits[partIndex]) != signatureBits[partIndex] ) {
					match = false;
					break;
				}

			}
			if( match )
				callback(signatureIndex);
		}

	}


	void SignatureArray::setSignatureBit(unsigned int signatureIndex, unsigned int bitIndex) {
		if( !(signatureIndex < numSignatures) )
			throw new IndexOutOfBoundsException(signatureIndex, numSignatures);

		// TODO: Consider adding a bit out of bounds

		bitManipulator.set(signatureIndex*signatureParts*8 + bitIndex);
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
		

		// - - - - + + + + - - - - + + + + - - - -

		// - - - - - + + + + + - - - - - + + + + + - - - - -
		
		// Check if more parts are required (i.e. we go from 8 to 9 in size)
		unsigned int newSignatureParts = 1 + (signatureSize - 1) / 8;
		unsigned int partsDifference = newSignatureParts - signatureParts;

		if( partsDifference > 0 ) {

			// Extend memory with memory for new parts
			reserveMemory(newSignatureParts * numSignatures);

			// Pointers to last part of last signature in old and new data
			unsigned char* oldData = data + (signatureParts-1) + (numSignatures-1) * signatureParts;
			unsigned char* newData = data + (newSignatureParts-1) + (numSignatures-1) * newSignatureParts;

			// Iterate over number of signatures backwards
			for( int signatureIndex = numSignatures - 1; signatureIndex >= 0; signatureIndex-- ) {
				int partIndex = newSignatureParts - 1;

				// Add new empty parts
				for( int i = 0; i < partsDifference; i++ ) {
					*newData = 0;
					newData--;
					partIndex--;
				}

				// Add old existing parts
				for( int i = 0; i < signatureParts; i++ ) {
					*newData = *oldData;
					newData--;
					oldData--;
				}
			}

			signatureParts = newSignatureParts;
		}

		signatureSize = newSignatureSize;
		bitManipulator.setData(data, numSignatures * signatureParts * 8);

		// TODO: Invalidate caches
	}




	unsigned int SignatureArray::add() {
		// TODO: Invalidate caches
		reserveSignatures(++numSignatures);

		// Initialize new signature to 0
		unsigned int signatureIndex = numSignatures - 1;
		for( unsigned int i = signatureIndex * signatureParts; i < numSignatures * signatureParts; i++ )
			data[i] = 0;

		bitManipulator.setData(data, numSignatures * signatureParts * 8);

		return signatureIndex;
	}


	unsigned int SignatureArray::remove(unsigned int index) {
		// TODO: Invalidate caches
		if( index <= numSignatures )
			throw new IndexOutOfBoundsException(index, numSignatures);

		numSignatures--;
		if( numSignatures == 0 || numSignatures == index ) return 0;
		
		// Move last signature to removed signatures slot
		unsigned char* lastSignature = data + (size_t)numSignatures * signatureParts;
		unsigned char* deletedSignature = data + (size_t)index * signatureParts;
		for( int part = 0; part < signatureParts; part++ )
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




/*
//TODO: REMOVE THIS

	void SignatureArray::setSignatureSizeOld(unsigned int newSignatureSize) {
		if( newSignatureSize == signatureSize )
			return;

		if( newSignatureSize < signatureSize )
			throw new SignatureSizeReducedException(signatureSize, newSignatureSize);


		// - - - - + + + + - - - - + + + + - - - -

		// - - - - - + + + + + - - - - - + + + + + - - - - -

		// TODO: This has not been tested yet

		// Check if more parts are required (i.e. we go from 8 to 9 in size)
		unsigned int newSignatureParts = 1 + (signatureSize - 1) / 8;
		unsigned int partsDifference = newSignatureParts - signatureParts;
		if( partsDifference > 0 ) {

			// Create new data list, which has the adjusted number of parts
			std::vector<unsigned char> newData;
			newData.reserve(newSignatureParts * numSignatures);

			auto currentDataIterator = signatureData.begin();
			for( int i = 0; i < numSignatures; i++ ) {
				// Copy existing data
				for( int j = 0; j < signatureParts; j++ ) {
					newData.push_back(*currentDataIterator);
					currentDataIterator++;
				}

				// Push new (empty) parts
				for( int j = 0; j < partsDifference; j++ ) {
					newData.push_back(0);
				}
			}

			signatureData = std::move(newData);
			signatureParts = newSignatureParts;
		}


		signatureSize = newSignatureSize;

		// TODO: Invalidate caches
	}

*/