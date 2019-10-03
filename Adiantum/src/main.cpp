#include "block.h"
#include "ChaCha.h"
#include "common.h"
#include "driver.h"

using namespace block;

int main(int argc, const char** argv)
{
	try {
		Driver driver(argc, argv);
		driver.launch();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}