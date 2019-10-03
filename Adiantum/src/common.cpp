#include "common.h"

static size_t open_stream(std::ifstream& str, const std::string& file_name)
{
	str.open(file_name, std::ios_base::binary);
	if (!str) {
		throw std::runtime_error(file_name + ": file doesn't exist");
	}
	size_t size;
	str.seekg(0, std::ios_base::end);
	size = str.tellg();
	str.seekg(0, std::ios_base::beg);
	return size;
}

static std::shared_ptr<uint8_t> extract_data(const std::string& file_name, size_t len, size_t& data_size)
{
	std::ifstream input;
	data_size = open_stream(input, file_name);
	std::shared_ptr<uint8_t> ptr(new uint8_t[len]);
	input.read((char*)ptr.get(), len);
	return ptr;
}

std::shared_ptr<uint8_t> extract_key(const std::string& file_name, size_t key_len)
{
	size_t real_len = 0;
	auto key_ptr = extract_data(file_name, key_len, real_len);
	if (real_len != key_len) {
		throw std::runtime_error("Incorrect key format");
	}
	return key_ptr;
}

std::shared_ptr<uint8_t> extract_iv(const std::string& file_name, size_t len)
{
	size_t real_len = 0;
	auto iv_ptr = extract_data(file_name, len, real_len);
	if (real_len != len) {
		throw std::runtime_error("Incorrect initial value");
	}
	return iv_ptr;
}