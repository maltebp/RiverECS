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
		memorySize = 0;
	}


	
	void SignatureArray::forMatchingSignatures(Signature& signature, std::function<void(unsigned int signatureIndex)> callback) {

		/*	There is no reason to check parts if they are all 0's, so we skip ahead and cut off the end */
		unsigned int firstPartIndex = signature.getFirstSetBit() < 0 ? 0 : signature.getFirstSetBit() / 8;
		unsigned int lastPartIndex = signature.getLastSetBit() < 0 ? 0 : signature.getLastSetBit() / 8;

		if( (signatureParts - 1) < lastPartIndex )
			return;// TODO: Throw exception: they don't match

		// First check if the query has been cached
		bool wasCached = checkCachedQuery(signature, callback);
		if( wasCached ) return;

		// Setup cache
		cachedQueries.emplace_back(std::make_pair(signature, std::vector<unsigned int>()));
		auto& cachedSignatureIndices = cachedQueries.back().second;

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

			if( match ) cachedSignatureIndices.push_back(signatureIndex);
		}

		// Run the callback for all the signature indices we just found (and
		// stored in cachedSignatureIndices
		for( auto signatureIndex : cachedSignatureIndices ) callback(signatureIndex);
	}



	bool SignatureArray::checkCachedQuery(Signature& signature, std::function<void(unsigned int signatureIndex)> callback) {
		int firstBit = signature.getFirstSetBit();
		int lastBit = signature.getLastSetBit();
		int numSetBits = signature.getBitsSet();
		unsigned int firstPartIndex = firstBit < 0 ? 0 : firstBit / 8;
		unsigned int lastPartIndex = lastBit < 0 ? 0 : lastBit / 8;

		unsigned char* signatureBits = signature.getBits();

		// For each cached query
		for( auto cachedQuery : cachedQueries ) {
			auto& cachedSignature = cachedQuery.first;

			// The signature can't match if either of these are not true, so we can just continue
			if( firstBit	!= cachedSignature.getFirstSetBit() ||
				lastBit		!= cachedSignature.getLastSetBit()	||
				numSetBits	!= cachedSignature.getBitsSet()
			) continue;
			
			// For each part in the signature to check for match
			auto cachedSignatureBits = cachedSignature.getBits();
			bool match = true;
			for( unsigned int partIndex = firstPartIndex; partIndex < lastPartIndex + 1; partIndex++ ) {
				// The Signatures has to exact matches, so each part must have the same value
				if( cachedSignatureBits[partIndex] != signatureBits[partIndex] ) {
					match = false;
					break;
				}
			}

			if( match ) {
				// Run callback for each signature index cached for the
				// matching Signature query
				for( auto signatureIndex : cachedQuery.second ) {
					callback(signatureIndex);
				}
				return true;
			}
		}
		return false;
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
		if( index >= numSignatures )
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