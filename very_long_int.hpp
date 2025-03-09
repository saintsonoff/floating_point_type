#ifndef _VERY_LONG_ING_HPP_
#define _VERY_LONG_ING_HPP_

#include <cstddef>
#include <cstdint>
#include <string>
#include <bitset>
#include <stdexcept>
#include <algorithm>

#include "bit_count.hpp"

namespace fl_p_detail {

template<size_t kSize>
class uint_n_t {
  public:
    uint_n_t() : data_m{} {  };
    uint_n_t(uint64_t data) : data_m{data} {  };
    uint_n_t(const std::bitset<kSize>& data) : data_m(data) {  };
    uint_n_t(std::bitset<kSize>&& data) : data_m(std::move(data)) {  };

    template<typename IntegerType>
    uint_n_t(IntegerType data) : data_m(data) {  };

    ~uint_n_t() {  };


  public:
    uint_n_t& operator+=(const uint_n_t& value) {
        bool digit_overflow = false;

        for (size_t bit_pos = 0; bit_pos < kSize; ++bit_pos) {
            bool fst_bit = data_m.test(bit_pos);
            bool scd_bit = value.data_m.test(bit_pos);

            data_m.set(bit_pos, fst_bit ^ scd_bit ^ digit_overflow);
            digit_overflow = (fst_bit && scd_bit) || (fst_bit && digit_overflow) || (scd_bit && digit_overflow);
        }
        return *this;
    }


    uint_n_t& operator-=(const uint_n_t& value) {
        *this += ~value + uint_n_t{0b1};
        return *this;
    }


    uint_n_t& operator*=(const uint_n_t& value) {
        for (size_t bit_pos = 0; bit_pos < kSize; ++bit_pos) {
            if (data_m.test(bit_pos))
                *this += value << bit_pos;
        }
        return *this;
    }

    uint_n_t& operator/=(const uint_n_t& value) {
        if (value == uint_n_t{0b0}) {
            throw std::domain_error("div_by_zero");
        }

        uint_n_t remainder;
        for (int bit_pos = kSize - 1; bit_pos >= 0; --bit_pos) {
            remainder <<= 1;
            remainder.data_m.set(0, data_m.test(bit_pos));
            if (remainder >= value) {
                remainder = remainder - value;
                data_m.set(bit_pos);
            }
        }
        return *this;
    }

    uint_n_t& operator%=(const uint_n_t& value) {
        if (value.data_m.none())
            throw std::domain_error("mod_by_zero");

        for (int i = kSize - 1; i >= 0; --i) {
            *this <<= 1;
            data_m.set(0, data_m.test(i));
            if (*this >= value) {
                *this -= value;
            }
        }
        return *this;
    }


  public:
    uint_n_t operator+(const uint_n_t& value) const {
        uint_n_t result = *this;
        result += value;
        return result;
    }


    uint_n_t operator-(const uint_n_t& value) const {
        uint_n_t result = *this;
        result -= value;
        return result;
    }


    uint_n_t operator*(const uint_n_t& value) const {
        uint_n_t result = *this;
        result *= value;
        return result;
    }

    uint_n_t operator/(const uint_n_t& value) const {
        uint_n_t result = *this;
        result /= value;
        return result;
    }

    uint_n_t operator%(const uint_n_t& value) const {
        uint_n_t result = *this;
        result %= value;
        return result;
    }


  public:

    template<typename IntType>
    uint_n_t operator>>(IntType offset) { return {data_m >> offset}; };
    template<typename IntType>
    uint_n_t operator<<(IntType offset) { return {data_m << offset}; };
    

  public:
    uint_n_t operator|=(const uint_n_t& value) {
        data_m |= value.data_m;
        return *this;
    };

    uint_n_t operator&=(const uint_n_t& value) {
        data_m &= value.data_m;
        return *this;
    };

    uint_n_t operator^=(const uint_n_t& value) {
        data_m ^= value.data_m;
        return *this;
    };


    uint_n_t operator|(const uint_n_t& value) const {
        uint_n_t current_value = *this;
        current_value |= value;
        return current_value; 
    };

    uint_n_t operator&(const uint_n_t& value) const {
        uint_n_t current_value = *this;
        current_value &= value;
        return current_value; 
    };

    uint_n_t operator^(const uint_n_t& value) const {
        uint_n_t current_value = *this;
        current_value ^= value;
        return current_value; 
    };


    uint_n_t operator~() const { return {~data_m}; };
    

  public:
    operator uint64_t() const {
        return (data_m & decltype(data_m){~uint64_t{0b0}}).to_ullong();
    }

    std::string to_string() const {
        return data_m.to_string();
    }


  public:
    friend std::strong_ordering operator<=>(const uint_n_t& lhs, const uint_n_t& rhs) {
        for (size_t bit_pos = 0; bit_pos < kSize; ++bit_pos) {
            if (lhs.data_m.test(kSize - 1 - bit_pos) != rhs.data_m.test(kSize - 1 - bit_pos)) {
                return lhs.data_m.test(kSize - 1 - bit_pos) ? std::strong_ordering::greater : std::strong_ordering::less;
            }
        }
        return std::strong_ordering::equal;
    }

    friend bool operator==(const uint_n_t& lhs, const uint_n_t& rhs) {
        return lhs.data_m == rhs.data_m;
    };


  private:
    std::bitset<kSize> data_m;
};


// baned zero size
template<>
class uint_n_t<0> {  };


} // namespace fl_p_detail

template<size_t kSize>
struct bit_count<fl_p_detail::uint_n_t<kSize>> {
    static constexpr size_t value = kSize;
};

#endif // _VERY_LONG_ING_HPP_