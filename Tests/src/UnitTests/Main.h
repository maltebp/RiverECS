#pragma once

#define CATCH_CONFIG_RUNNER
#include <catch.h>
#include <string>




// Test case files -------------------------------------
#include "ComponentController.h"
#include "Signature/BitManipulator.h"
#include "Signature/Signature.h"
#include "Signature/SignatureArray.h"
#include "Entity/Entity.h"
// -----------------------------------------------------





std::vector<std::string> toStringList(int argc, char* argv[]) {
	std::vector<std::string> list;

	for( int i = 0; i < argc; i++ ) {
		char* c_string = argv[i];
		list.push_back(std::string(c_string));
	}
	return list;
}

std::vector<char*> toCStringList(std::vector<std::string>& strings) {

	std::vector<char*> cStrings;
	for( auto& string : strings ) {
		char* cString = new char[string.size() + 1];
		strcpy_s(cString, string.size()+1, string.c_str());
		cStrings.push_back(cString);
	}

	return cStrings;
}



int start(int argc, char* argv[]) {

	auto args = toStringList(argc, argv);
	

	// Add test arguments here (i.e. tag on test cases to run)
	args.push_back("[entity]");
	args.push_back("-b");


	auto cArgs = toCStringList(args);
	int result = Catch::Session().run(cArgs.size(), &cArgs[0]);

	return result;
}
