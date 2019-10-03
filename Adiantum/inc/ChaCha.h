#pragma once

#include "block.h"

namespace chacha
{

	using namespace block;

	enum {
		KEY_LEN = 32,
		IV_LEN = 12
	};

	class State
	{

	public:
		State(const uint8_t*, const uint8_t*, uint32_t);
		State(const Block<256>&, const Block<96>, uint32_t);
		State(const Block<512>&) noexcept;
		Block<512> get_block() noexcept;
		void inc_block_counter() noexcept;
		friend std::ostream& operator<< (std::ostream&, const State&);

	private:
		uint32_t state[16] = { 0 };
		void set_constants() noexcept;
		void set_key(const uint8_t*) noexcept;
		void set_key(const Block<256>&) noexcept;
		void set_block_cnt(uint32_t) noexcept;
		void set_iv(const uint8_t*) noexcept;
		void set_iv(const Block<96>&) noexcept;
		void quater_round(uint32_t&, uint32_t&, uint32_t&, uint32_t&) noexcept;
		void round() noexcept;
		uint32_t lrot(uint32_t, short) const noexcept;

	};

	class Context
	{
		
	public:
		Context(const uint8_t*, const uint8_t*);
		size_t update(const uint8_t*, size_t) noexcept;
		void flush(std::ostream&);

	private:
		enum {
			BUF_SIZE = 1024
		};
		uint8_t buf[BUF_SIZE] = { 0 };
		State state;
		size_t data_len_in_buf = 0;

	};

	void ChaCha20(const uint8_t*, const uint8_t*, const std::string&, 
		const std::string& = "ciphertexts/encrypted_msg.txt");

}