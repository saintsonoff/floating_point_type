#ifndef _FLOAT_NUMBER_HPP_
#define _FLOAT_NUMBER_HPP_

#include <string>
#include <string_view>
#include <sstream>
#include <cstddef>
#include <cstdint>
#include <iomanip>

enum RoundingType { TOWARD_ZERO, TOWARD_EVEN, TOWARD_POS_INF, TOWARD_NEG_INF };

template<RoundingType kRounding>
class RoundingDevice {
  protected:
    uint64_t rounding() {

    };
};


template<RoundingType kRounding>
class FloatingPointNumber : RoundingDevice<kRounding> {
  public:
    using IntegerType = uint32_t;


  public:
    FloatingPointNumber(std::string_view presision, IntegerType data) : data_m(data) {
        if (presision == "h") {
            mant_size_m = 10;
            exp_size_m = 5;
        } else if (presision == "s") {
            mant_size_m = 23;
            exp_size_m = 8;
        } else {
            is_valid_m = false;
        }
    };


    FloatingPointNumber(IntegerType data, IntegerType mant_size, IntegerType exp_size)
      : data_m(data), mant_size_m(mant_size), exp_size_m(exp_size) {  };



  private:
    IntegerType exp_mask() const { return ((0b1 << exp_size_m) - 1) << mant_size_m; };
    IntegerType mant_mask() const { return (0b1 << mant_size_m) - 1; };

    IntegerType get_exp() const { return (data_m & exp_mask()) >> mant_size_m; }

    int32_t get_exp_value() const { 
        int32_t exp_value = get_exp();
        exp_value -= ((0b1 << (exp_size_m - 1)) - 1);
        return exp_value;
    }

    IntegerType get_mant() const { return data_m & mant_mask(); }
    
    std::pair<IntegerType, int32_t> get_normalize() const {
        if (!is_denormalized()) {
            return {get_mant(), get_exp_value()};
        }

        IntegerType mant = get_mant();

        int normalize_offset = 0;;
        while (((mant << normalize_offset) & exp_mask()) == 0) {
            ++normalize_offset;
        }

        return {(mant << normalize_offset) & mant_mask(), -(normalize_offset - 1) - ((0b1 << (exp_size_m - 1)) - 1)};
    }

    IntegerType is_negative() const { return data_m & (0b1 << (exp_size_m + mant_size_m)); }

    bool is_nan() const { return get_exp() == (exp_mask() >> mant_size_m) && get_mant() != IntegerType{0b0}; };
    bool is_inf() const { return get_exp() == (exp_mask() >> mant_size_m) && get_mant() == IntegerType{0b0}; };
    bool is_zero() const { return (data_m & (exp_mask() | mant_mask())) == IntegerType{0}; };
    bool is_denormalized() const { return (data_m & exp_mask()) == 0 && !is_zero(); }; 

    std::string to_string() const;

  public:
    std::string output_labwork_task() const {
        std::stringstream result_ss;

        result_ss << to_string();
        // result_ss << " ";
        // result_ss << "0x" << std::uppercase << std::hex << data_m;

        return result_ss.str();
    };

  public:
    bool is_valid() { return is_valid_m; };


  private:
    bool is_valid_m = true;
    IntegerType mant_size_m;
    IntegerType exp_size_m;
    IntegerType data_m;
};



template<RoundingType kRounding>
std::string FloatingPointNumber<kRounding>::to_string() const {
    std::stringstream result_ss;

    int mant_offset = 0;
    while ((mant_size_m + mant_offset) % 4) {
        ++mant_offset;
    }

    if (is_nan()) {
        return "nan";
    }

    if (is_inf()) {
        if (!is_negative()) {
            return "inf";
        } else {
            return "-inf";
        }
    }

    if (is_zero()) {
        if (is_negative()) {
            result_ss << "-";
        }

        result_ss << "0x0.";
        result_ss
            << std::nouppercase << std::hex
            << std::setw((mant_size_m + mant_offset) / 4) << std::setfill('0') << "0";
        result_ss << "p+0";
        return result_ss.str();
    }

    if (is_negative()) {
        result_ss << "-";
    }

    result_ss << "0x1.";

    auto [mantisa, exponent] = get_normalize();

    result_ss << std::nouppercase << std::hex
        << std::setw((mant_size_m + mant_offset) / 4) << std::setfill('0')
        << (mantisa << mant_offset);

    result_ss << "p";

    if (exponent >= 0) {
        result_ss << "+";
    }

    result_ss << std::dec << exponent;

    return result_ss.str();
}

#endif // _FLOAT_NUMBER_HPP_