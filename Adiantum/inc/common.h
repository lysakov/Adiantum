#pragma once

#include <fstream>
#include <memory>
#include <iostream>

std::shared_ptr<uint8_t> extract_key(const std::string&, size_t);
std::shared_ptr<uint8_t> extract_iv(const std::string&, size_t);
