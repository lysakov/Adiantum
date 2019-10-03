#pragma once

#include <memory>
#include "ChaCha.h"
#include "common.h"

class Driver
{

public:
	Driver(int, const char**);
	void launch() const;

private:
	enum {
		INPUT = 1,
		KEY = 2,
		IV = 3,
		OUTPUT = 4
	};
	const char** param;
	bool output = false;

};

