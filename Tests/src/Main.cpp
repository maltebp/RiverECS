
#define UNIT_TESTS	1


#if UNIT_TESTS == 0
	#include "Sandbox/Main.h"
#else
	#include "UnitTests/Main.h"
#endif	


int main(int argc, char* argv[]) {
	return start(argc, argv);
}
