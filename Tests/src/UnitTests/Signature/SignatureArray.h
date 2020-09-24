#pragma once

#include <catch.h>

#include <ECS/SignatureArray/SignatureArray.h>


TEST_CASE("Testing adding signatures, and settings values", "[signature_array]") {

	ECS::SignatureArray signatureArray(1000);
	signatureArray.setSignatureSize(5);

	auto signature1 = signatureArray.add();
	signatureArray.setSignatureBit(signature1, 0);
	signatureArray.setSignatureBit(signature1, 2);
	signatureArray.setSignatureBit(signature1, 4);

	auto signature2 = signatureArray.add();
	signatureArray.setSignatureBit(signature2, 1);
	signatureArray.setSignatureBit(signature2, 5);
	
	auto signature3 = signatureArray.add();
	
	auto signature4 = signatureArray.add();
	signatureArray.setSignatureBit(signature4, 0);
	signatureArray.setSignatureBit(signature4, 1);
	signatureArray.setSignatureBit(signature4, 2);
	signatureArray.setSignatureBit(signature4, 3);
	signatureArray.setSignatureBit(signature4, 4);

	
	REQUIRE(signatureArray.getSignatureBit(signature1, 0));
	REQUIRE(signatureArray.getSignatureBit(signature1, 2));
	REQUIRE(signatureArray.getSignatureBit(signature1, 4));

}