#pragma once

#include <vector>
#include <set>
#include <catch.h>

#include <ECS/SignatureArray/SignatureArray.h>


TEST_CASE("Testing adding signatures, and settings values", "[signature_array]") {

	ECS::SignatureArray signatureArray(1000);
	signatureArray.setSignatureSize(10);

	// List of sets of signatures bits to set
	std::vector<std::set<unsigned int>> signatures{
		{0, 2, 5, 7},
		{0, 1, 5},
		{7, 8, 9},
		{0, 1, 2, 3, 4}
	};

	// Setting bits for each signature
	for( int i = 0; i < signatures.size(); i++ ) {
		auto signatureIndex = signatureArray.add();
		for( auto& bit : signatures[i] ) {
			signatureArray.setSignatureBit(signatureIndex, bit);
		}
	}

	// Checking all bits for each signature (0-9)
	for( int i = 0; i < signatures.size(); i++ ) {
		for( int bit = 0; bit < 10; bit++ ) {
			REQUIRE(signatureArray.getSignatureBit(i, bit) == (signatures[i].find(bit) != signatures[i].end()));
		}
	}
}



TEST_CASE("Testing unsetting values", "[signature_array]") {
	/*
		Setting 4 signatures' bits, unsetting them and checking
		correct values.
	*/

	ECS::SignatureArray signatureArray(1000);
	signatureArray.setSignatureSize(10);

	// List of sets of signatures bits to set
	std::vector<std::set<unsigned int>> bitsToSet {
		{ 0, 2, 5, 7 },
		{ 0, 1, 5 },
		{ 7, 8 },
		{ 0, 1, 2, 3, 4 }
	};

	// List of sets of signature's bits to unset
	std::vector<std::set<unsigned int>> bitsToUnset {
		{0, 7},
		{1 },
		{7, 8},
		{ 0, 1, 2, 3, 4 }
	};

	// Setting bits for each signature
	for( int i = 0; i < bitsToSet.size(); i++ ) {
		auto signatureIndex = signatureArray.add();
		for( auto& bit : bitsToSet[i] ) {
			signatureArray.setSignatureBit(signatureIndex, bit);
		}
	}

	// Unsettng bits
	for( int i = 0; i < bitsToUnset.size(); i++ ) {
		for( auto& bit : bitsToUnset[i] ) {
			signatureArray.unsetSignatureBit(i, bit);
			bitsToSet[i].erase(bitsToSet[i].find(bit));
		}
	}

	// Checking all bits for each signature (0-9)
	for( int i = 0; i < bitsToSet.size(); i++ ) {
		for( int bit = 0; bit < 10; bit++ ) {
			REQUIRE(signatureArray.getSignatureBit(i, bit) == (bitsToSet[i].find(bit) != bitsToSet[i].end()));
		}
	}
}



TEST_CASE("Testing removal of signature", "[signature_array]") {
	/*
		Setting 4 signatures' bits, resizing the signatures and
		checking them again
	*/

	ECS::SignatureArray signatureArray(1000);
	signatureArray.setSignatureSize(10);

	// List of sets of signatures bits to set
	std::vector<std::set<unsigned int>> bitsToSet{
		{ 0, 2, 5, 7 },
		{ 0, 1, 5 },
		{ 7, 8, 9 },
		{ 0, 1, 2, 3, 4 }
	};

	// Setting bits for each signature
	for( int i = 0; i < bitsToSet.size(); i++ ) {
		auto signatureIndex = signatureArray.add();
		for( auto& bit : bitsToSet[i] ) {
			signatureArray.setSignatureBit(signatureIndex, bit);
		}
	}

	REQUIRE(signatureArray.getNumSignatures() == 4);

	// Checking all bits for each signature (0-9)
	for( int i = 0; i < bitsToSet.size(); i++ ) {
		for( int bit = 0; bit < 10; bit++ ) {
			REQUIRE(signatureArray.getSignatureBit(i, bit) == (bitsToSet[i].find(bit) != bitsToSet[i].end()));
		}
	}


	SECTION("Removing index 0") {
		unsigned int index = 0;
		signatureArray.remove(index);
		auto element = bitsToSet.erase(bitsToSet.begin() + index);
		auto last = (bitsToSet.end() - 1);
		bitsToSet.insert(bitsToSet.begin()+index, *last);
		bitsToSet.erase(bitsToSet.end() - 1);
	}

	SECTION("Removing index 1") {
		unsigned int index = 1;
		signatureArray.remove(index);
		auto element = bitsToSet.erase(bitsToSet.begin() + index);
		auto last = (bitsToSet.end() - 1);
		bitsToSet.insert(bitsToSet.begin() + index, *last);
		bitsToSet.erase(bitsToSet.end() - 1);
	}

	SECTION("Removing index 2") {
		unsigned int index = 2;
		signatureArray.remove(index);
		auto element = bitsToSet.erase(bitsToSet.begin() + index);
		auto last = (bitsToSet.end() - 1);
		bitsToSet.insert(bitsToSet.begin() + index, *last);
		bitsToSet.erase(bitsToSet.end() - 1);
	}

	SECTION("Removing index 3") {
		unsigned int index = 3;
		signatureArray.remove(index);
		auto element = bitsToSet.erase(bitsToSet.begin() + index);
		auto last = (bitsToSet.end() - 1);
		bitsToSet.insert(bitsToSet.begin() + index, *last);
		bitsToSet.erase(bitsToSet.end() - 1);
	}

	SECTION("Removing all") {
		signatureArray.remove(0);
		signatureArray.remove(0);
		signatureArray.remove(0);
		signatureArray.remove(0);
		bitsToSet.clear();
	}

	REQUIRE(signatureArray.getNumSignatures() == bitsToSet.size());
	
	// Checking all bits for each signature (0-9)
	for( int i = 0; i < bitsToSet.size(); i++ ) {
		for( int bit = 0; bit < 10; bit++ ) {
			REQUIRE(signatureArray.getSignatureBit(i, bit) == (bitsToSet[i].find(bit) != bitsToSet[i].end()));
		}
	}
}



TEST_CASE("Testing resing array", "[signature_array]") {
	/*
		Setting 4 signatures' bits, resizing the signatures and
		checking them again
	*/

	ECS::SignatureArray signatureArray(1000);
	signatureArray.setSignatureSize(10);

	// List of sets of signatures bits to set
	std::vector<std::set<unsigned int>> bitsToSet{
		{ 0, 2, 5, 7 },
		{ 0, 1, 5 },
		{ 7, 8, 9 },
		{ 0, 1, 2, 3, 4 }
	};

	// Setting bits for each signature
	for( int i = 0; i < bitsToSet.size(); i++ ) {
		auto signatureIndex = signatureArray.add();
		for( auto& bit : bitsToSet[i] ) {
			signatureArray.setSignatureBit(signatureIndex, bit);
		}
	}

	// Checking all bits for each signature (0-9)
	for( int i = 0; i < bitsToSet.size(); i++ ) {
		for( int bit = 0; bit < 10; bit++ ) {
			REQUIRE(signatureArray.getSignatureBit(i, bit) == (bitsToSet[i].find(bit) != bitsToSet[i].end()));
		}
	}

	signatureArray.setSignatureSize(100);

	// List of sets of signatures bits to set
	std::vector<std::set<unsigned int>> bitsToSet2{
		{ 56 },
		{ 63, 64, 99 },
		{ },
		{ 39, 20, 80 }
	};

	// Setting additional bits for each signature
	for( int i = 0; i < bitsToSet2.size(); i++ ) {
		for( auto& bit : bitsToSet2[i] ) {
			signatureArray.setSignatureBit(i, bit);
			bitsToSet[i].emplace(bit);
		}
	}

	// Checking all bits for each signature (0-9)
	for( int i = 0; i < bitsToSet.size(); i++ ) {
		for( int bit = 0; bit < 100; bit++ ) {
			REQUIRE(signatureArray.getSignatureBit(i, bit) == (bitsToSet[i].find(bit) != bitsToSet[i].end()));
		}
	}	
}


TEST_CASE("Testing reallocation of array", "[signature_array]") {
	ECS::SignatureArray signatureArray(15);
	signatureArray.setSignatureSize(10);

	// List of sets of signatures bits to set
	std::vector<std::set<unsigned int>> signatures{
		{ 0, 2, 5, 7 },
		{ 0, 1, 5 },
		{ 7, 8, 9 },
		{ 0, 1, 2, 3, 4 }
	};

	// Setting bits for each signature
	for( int i = 0; i < signatures.size(); i++ ) {
		auto signatureIndex = signatureArray.add();
		for( auto& bit : signatures[i] ) {
			signatureArray.setSignatureBit(signatureIndex, bit);
		}
	}

	// Checking all bits for each signature (0-9)
	for( int i = 0; i < signatures.size(); i++ ) {
		for( int bit = 0; bit < 10; bit++ ) {
			REQUIRE(signatureArray.getSignatureBit(i, bit) == (signatures[i].find(bit) != signatures[i].end()));
		}
	}
}




