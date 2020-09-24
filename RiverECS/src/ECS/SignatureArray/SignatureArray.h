#pragma once

#include <vector>
#include <functional>
#include <sstream>

#include "Signature.h"
#include <unordered_map>
#include <ECS.h>

namespace ECS {

	// Thrown if the all compnent id has been used 
	class SignatureSizeReducedException : public Exception {
	public:
		SignatureSizeReducedException(int previousValue, int newValue) :
			Exception("Cannot reduce signature size (from "+std::to_string(previousValue) +" to " + std::to_string(newValue))
		{}
	};

	class SignatureArray {
	public:

		SignatureArray();
		~SignatureArray();

		/**
		 * @brief Removes all signatures from the array
		 *
		 * This deallocates the dynamic memory allocated for this list, and sets the number of
		 * contained signatures to 0.
		 * The signature size is not altered.
		*/
		void clear();

		/**
		 * @brief Checks if each signature in the array matches the given signature, and call the callback function in case it does
		 * @param signature
		 * @param callback
		*/
		void forMatchingSignatures(Signature& signature, std::function<void(unsigned int signatureIndex)> callback);


		void setSignatureBit(unsigned int signatureIndex, unsigned int bitIndex);
		void unsetSignatureBit(unsigned int signatureIndex, unsigned int bitIndex);
		void unsetAllSignatureBit(unsigned int signatureIndex, unsigned int bitIndex);


		void setSignatureSize(unsigned int newSignatureSize);

		/**
		 * @brief Adds a new signature to the array, where all bits are set to 0
		*/
		void add();


	private:
		SignatureArray& operator=(SignatureArray& other) = delete;
		SignatureArray(const SignatureArray& other) = delete;


	private:
		unsigned int numSignatures = 0;

		unsigned int signatureSize = 1;
		unsigned int signatureParts = 1; // Number of unsigned chars to represent elements (always elementSize ceil(elementSize/sizeof(unsigned char))

		unsigned char* data = nullptr;

		// Maps signature to index to Entity
		std::unordered_map<unsigned int, ECS::Entity*> entityMap;

		// Maps Entity to signature index
		std::unordered_map<ECS::Entity*, unsigned int> indexMap;


		std::vector<unsigned char> signatureData;

		
	};


}

