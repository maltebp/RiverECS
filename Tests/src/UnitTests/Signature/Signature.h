#pragma once

#pragma once

#include <catch.h>


#include <ECS/SignatureArray/Signature.h>




TEST_CASE("Resizing Signature", "[signature]") {

	ECS::Signature signature(60);

	signature.set(4); 
	signature.set(6);
	signature.set(8);
	signature.set(1);
	signature.set(3);
	signature.set(24);
	signature.set(0);
	signature.set(30);
	signature.set(45);
	signature.set(43);
	signature.set(59);


	REQUIRE(signature.getSize() == 60);
	REQUIRE(signature.getBitsSet() == 11);
	REQUIRE(signature.getFirstSetBit() == 0);
	REQUIRE(signature.getLastSetBit() == 59);

	// Resize which won't require reallocation of memory
	signature.resize(62);

	REQUIRE(signature.getSize() == 62);
	REQUIRE(signature.getBitsSet() == 11);
	REQUIRE(signature.getFirstSetBit() == 0);
	REQUIRE(signature.getLastSetBit() == 59);

	signature.set(61);
	REQUIRE(signature.getBitsSet() == 12);
	REQUIRE(signature.getLastSetBit() == 61);


	// REsize which will cause reallaction of memory
	signature.resize(66);

	REQUIRE(signature.getSize() == 66);
	REQUIRE(signature.getBitsSet() == 12);
	REQUIRE(signature.getFirstSetBit() == 0);
	REQUIRE(signature.getLastSetBit() == 61);

	signature.set(65);
	REQUIRE(signature.getBitsSet() == 13);
	REQUIRE(signature.getLastSetBit() == 65);

}