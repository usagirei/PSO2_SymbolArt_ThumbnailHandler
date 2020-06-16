#pragma once

#include <cstdint>

namespace crypto_blowfish {

	namespace constants {

		extern uint32_t s0[];
		extern uint32_t s1[];
		extern uint32_t s2[];
		extern uint32_t s3[];
		extern uint32_t p[];
	}

	namespace util {

		template <size_t _S, typename _T = uint32_t>
		struct n_box {
		private:
			_T m_Data[_S];
		public:
			static constexpr size_t size = _S;
			using type = _T;

			_T& operator[](int n);
			n_box(_T d[_S]);
			n_box(_T* d, int s);
		};
	}

	class blowfish
	{
	public:
		using sbox = util::n_box<256, uint32_t>;
		using pbox = util::n_box<18, uint32_t>;
		using kbox = util::n_box<56, uint8_t>;

		void decrypt(uint8_t* dst, uint8_t* src, size_t count);
		void encrypt(uint8_t* dst, uint8_t* src, size_t count);
		blowfish(uint8_t* key, size_t len);

	private:
		inline uint32_t _round(uint32_t x);
		void _encrypt(uint32_t& l, uint32_t& r);
		void _decrypt(uint32_t& l, uint32_t& r);
		void init();

		int m_KLen;
		kbox m_K;
		sbox m_S[4];
		pbox m_P;
	};

}

#include <blowfish.inl>