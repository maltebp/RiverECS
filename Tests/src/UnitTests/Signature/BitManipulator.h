#pragma once

#include <catch.h>
#include <unordered_set>

#include <ECS/SignatureArray/BitManipulator.h>


void unset(unsigned char* data, int size) {
	for( int i = 0; i < size; i++ )
		data[i] = 0;
}



TEST_CASE("Generel tests", "[bit_manipulator]") {
	unsigned int bytes = 5;
	unsigned int bits = bytes * 8;
	unsigned char* data = new unsigned char[bytes]();
	River::ECS::BitManipulator manipulator(data, bits);

	SECTION("Set and unset each individual bit") {
		for( unsigned int i = 0; i < bits; i++ ) {
			manipulator.set(i);
			REQUIRE(manipulator.getBitsSet() == 1);
			REQUIRE(manipulator.get(i) == true);
			REQUIRE(manipulator.getFirstSetBit() == i);
			REQUIRE(manipulator.getLastSetBit() == i);
			manipulator.unset(i);
			REQUIRE(manipulator.getBitsSet() == 0);
			REQUIRE(manipulator.get(i) == false);
			REQUIRE(manipulator.getFirstSetBit() == -1);
			REQUIRE(manipulator.getLastSetBit() == -1);

		}
	}

	SECTION("Last and first bit") {
		REQUIRE(manipulator.getFirstSetBit() == -1);
		REQUIRE(manipulator.getLastSetBit() == -1);

		manipulator.set(1);
		manipulator.set(7);
		manipulator.set(10);
		REQUIRE(manipulator.getFirstSetBit() == 1);
		REQUIRE(manipulator.getLastSetBit() == 10);

		manipulator.unset(10);
		REQUIRE(manipulator.getFirstSetBit() == 1);
		REQUIRE(manipulator.getLastSetBit() == 7);

		manipulator.unset(1);
		REQUIRE(manipulator.getFirstSetBit() == 7);
		REQUIRE(manipulator.getLastSetBit() == 7);

		manipulator.set(30);
		REQUIRE(manipulator.getFirstSetBit() == 7);
		REQUIRE(manipulator.getLastSetBit() == 30);
	}


	SECTION("For each bit set") {

		unsigned int expectedBits[] = {
			4, 8, 14, 15, 17, 30, 31
		};

		for( auto& bit : expectedBits ) {
			manipulator.set(bit);
		}

		std::unordered_set<unsigned int> bitsCallbacked;

		manipulator.forEachSetBit( [&bitsCallbacked](unsigned int bitIndex){
			REQUIRE(bitsCallbacked.find(bitIndex) == bitsCallbacked.end());
			bitsCallbacked.emplace(bitIndex);
		});

		for( auto& bit : expectedBits ) {
			REQUIRE(bitsCallbacked.find(bit) != bitsCallbacked.end());
			bitsCallbacked.erase(bit);
		}
		
		REQUIRE(bitsCallbacked.size() == 0);

	}




	delete[] data;
}