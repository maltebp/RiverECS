#pragma once

#include <vector>
#include <functional>
#include <sstream>

#include "SignatureArray.h"


namespace ECS {


	SignatureArray::SignatureArray() {}

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
			throw std::exception(("Index out of bounds (i=" + std::to_string(signatureIndex) + ", size=" + std::to_string(numSignatures) + ")").c_str());

		data[signatureIndex * signatureParts];// set(bitIndex);
	}




	void SignatureArray::setSignatureSize(unsigned int newSignatureSize) {
		if( newSignatureSize == signatureSize )
			return;

		if( newSignatureSize < signatureSize )
			throw new SignatureSizeReducedException(signatureSize, newSignatureSize);
		
		
		// TODO: This has not been tested yet

		// Check if more parts are required (i.e. we go from 8 to 9 in size)
		unsigned int newSignatureParts = 1 + (signatureSize - 1) / 8;
		unsigned int partsDifference = newSignatureParts - signatureParts;
		if( partsDifference > 0 ) {

			// Create new data list, which has the adjusted number of parts
			std::vector<unsigned char> newData;
			newData.reserve(newSignatureParts*numSignatures);
			
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


	void SignatureArray::add() {
		auto newData = (unsigned char*)realloc(data, sizeof(unsigned char) * (numSignatures + 1) * signatureParts);
		if( newData == nullptr )
			// TODO: Check for null pointer
			throw std::exception("Could not resize the SignatureArray");

		numSignatures++;
		data = newData;

		// Initialize new parts to 0
		for( unsigned int i = (numSignatures - 1) * signatureParts; i < numSignatures * signatureParts; i++ )
			data[i] = 0;
	}

}
