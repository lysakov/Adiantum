#include "driver.h"

Driver::Driver(int argc, const char** argv) : param(argv)
{
	if (argc > 5 && argc < 4) {
		throw std::runtime_error("Wrong number of parametrs");
	}
	if (argc == 5) {
		output = true;
	}
}

void Driver::launch() const
{
	auto key_blob = extract_key(param[KEY], chacha::KEY_LEN);
	auto iv = extract_iv(param[IV], chacha::IV_LEN);
	if (output) {
		chacha::ChaCha20(key_blob.get(), iv.get(), param[INPUT], param[OUTPUT]);
	}
	else {
		chacha::ChaCha20(key_blob.get(), iv.get(), param[INPUT]);
	}
}
