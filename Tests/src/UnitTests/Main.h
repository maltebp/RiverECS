#pragma once

#define CATCH_CONFIG_RUNNER
#include <catch.h>

#include "ComponentController.h"
#include "Signature/BitManipulator.h"

int start(int argc, char* argv[]) {
	int result = Catch::Session().run(argc, argv);
	return result;
}
