#include "ChaCha.h"

using namespace chacha;
using namespace block;

static uint32_t little_endian(uint32_t);

State::State(const uint8_t* key, const uint8_t* iv, uint32_t block_cnt)
{
	set_constants();
	if (key) {
		set_key(key);
	}
	else {
		throw std::runtime_error("Incorrect key format");
	}
	set_block_cnt(block_cnt);
	if (iv) {
		set_iv(iv);
	}
	else {
		throw std::runtime_error("Incorrext initial value");
	}
}

State::State(const Block<256>& key, const Block<96> iv, uint32_t block_cnt)
{
	set_constants();
	set_key(key);
	set_block_cnt(block_cnt);
	set_iv(iv);
}

State::State(const Block<512>& block) noexcept
{
	for (int i = 0; i < 64; ++i) {
		((uint8_t*)state)[i] = block.get_byte(i);
	}
}

Block<512> State::get_block() noexcept
{
	enum {
		ROUNDS_NUM = 10
	};
	State working_state = *this;
	for (int i = 0; i < ROUNDS_NUM; ++i) {
		working_state.round();
	}
	State new_state = *this;
	for (int i = 0; i < 16; ++i) {
		new_state.state[i] = (*this).state[i] + working_state.state[i];
	}
	return Block<512>((uint8_t*)new_state.state, 64);
}

void State::inc_block_counter() noexcept
{
	++state[12];
}

void State::set_constants() noexcept
{
	state[0] = 0x61707865;
	state[1] = 0x3320646e;
	state[2] = 0x79622d32; 
	state[3] = 0x6b206574;
}

void State::set_key(const uint8_t* key) noexcept
{
	for (int i = 0; i < 8; ++i) {
		state[4 + i] = *((const uint32_t*)key + i);
	}
}

void State::set_key(const Block<256>& key) noexcept
{
	for (int i = 0; i < 8; ++i) {
		state[4 + i] = little_endian(*((const uint32_t*)key.get_bitset() + 7 - i));
	}
}

void State::set_block_cnt(uint32_t num) noexcept
{
	state[12] = num;
}

void State::set_iv(const uint8_t* iv) noexcept
{
	for (int i = 0; i < 3; ++i) {
		state[13 + i] = *((const uint32_t*)iv + i);
	}
}

void chacha::State::set_iv(const Block<96>& iv) noexcept
{
	for (int i = 0; i < 3; ++i) {
		state[13 + i] = little_endian(*((const uint32_t*)iv.get_bitset() + 2 - i));
	}
}

void State::quater_round(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) noexcept
{
	a = a + b;
	d = d ^ a;
	d = lrot(d, 16);
	c = c + d;
	b = b ^ c;
	b = lrot(b, 12);
	a = a + b;
	d = d ^ a;
	d = lrot(d, 8);
	c = c + d;
	b = b ^ c;
	b = lrot(b, 7);
}

void State::round() noexcept
{
	quater_round(state[0], state[4], state[8], state[12]);
	quater_round(state[1], state[5], state[9], state[13]);
	quater_round(state[2], state[6], state[10], state[14]);
	quater_round(state[3], state[7], state[11], state[15]);
	quater_round(state[0], state[5], state[10], state[15]);
	quater_round(state[1], state[6], state[11], state[12]);
	quater_round(state[2], state[7], state[8], state[13]);
	quater_round(state[3], state[4], state[9], state[14]);
}

uint32_t State::lrot(uint32_t num, short shift_val) const noexcept
{
	return (num << shift_val) | (num >> (32 - shift_val));
}

Context::Context(const uint8_t* key, const uint8_t* iv) : state(key, iv, 1)
{
}

size_t Context::update(const uint8_t* data, size_t data_len) noexcept
{
	enum {
		BLOCK_SIZE = 64
	};
	int64_t len = data_len < BUF_SIZE ? data_len : BUF_SIZE;
	data_len_in_buf = len;
	uint8_t* cur_buf_ptr = buf;
	while (len > 0) {
		Block<512> gamma = state.get_block();
		const uint8_t* gamma_ptr = gamma.get_bitset();
		for (int i = 0; i < BLOCK_SIZE; ++i) {
			cur_buf_ptr[i] = data[i] ^ gamma_ptr[i];
		}
		cur_buf_ptr += BLOCK_SIZE;
		data += BLOCK_SIZE;
		state.inc_block_counter();
		len -= BLOCK_SIZE;
	}
	return data_len_in_buf;
}

void Context::flush(std::ostream& output)
{
	output.write((const char*)buf, data_len_in_buf);
}

std::ostream& chacha::operator<<(std::ostream& str, const State& state)
{
	str << std::hex;
	for (int i = 0; i < 16; ++i) {
		str << state.state[i] << ' ';
		if (i == 3 || i == 7 || i == 11) {
			str << std::endl;
		}
	}
	str << std::dec;
	return str;
}

void chacha::ChaCha20(const uint8_t* key, const uint8_t* iv, 
	const std::string& input_name, const std::string& output_name)
{
	enum {
		BUF_SIZE = 1024
	};
	std::ifstream input(input_name, std::ios_base::binary);
	std::ofstream output(output_name, std::ios_base::binary);
	if (!input) {
		throw std::runtime_error("File with plaintext doesn't exist");
	}
	if (!output) {
		throw std::runtime_error("Impossible to create file with ciphertext");
	}
	Context ctx(key, iv);
	uint8_t buf[BUF_SIZE] = { 0 };
	while (input) {
		size_t enc_data_len = 0;
		input.read((char*)buf, BUF_SIZE);
		size_t data_len = input.gcount();
		while (enc_data_len != data_len) {
			enc_data_len += ctx.update(buf + enc_data_len, data_len - enc_data_len);
			ctx.flush(output);
		}
	}
}

static uint32_t little_endian(uint32_t num)
{
	uint8_t* data = (uint8_t*)&num;
	return (data[3] << 0) | (data[2] << 8) | (data[1] << 16) | (data[0] << 24);
}