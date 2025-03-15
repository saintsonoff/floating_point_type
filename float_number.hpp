#ifndef _FLOAT_NUMBER_HPP_
#define _FLOAT_NUMBER_HPP_

#include <array>
#include <ios>
#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <functional>

#include "very_long_int.hpp"
#include "rounding_device.hpp"
#include "bit_count.hpp"

template<RoundingType kRounding = RoundingType::TOWARD_EVEN>
class FloatingPointNumber : RoundingDevice<kRounding, fl_p_detail::uint_n_t<512>> {
  public:
    using IntegerType = uint32_t;
    using MoreIntegerType = uint64_t;
    using MajorIntegerType = fl_p_detail::uint_n_t<512>;


  public:
    FloatingPointNumber(std::string_view presision, IntegerType data) : data_m{data} {
        if (presision == "h") {
            mant_size_m = 10;
            exp_size_m = 5;
        } else if (presision == "s" || presision == "f") {
            mant_size_m = 23;
            exp_size_m = 8;
        } else {
            is_valid_m = false;
        }
        data_m &= value_mask();
    };


    FloatingPointNumber(IntegerType data, IntegerType mant_size, IntegerType exp_size)
      : data_m(data), mant_size_m(mant_size), exp_size_m(exp_size) {  };



  private:
    IntegerType exp_mask() const { return ((MoreIntegerType{0b1} << exp_size_m) - 1) << mant_size_m; };
    IntegerType mant_mask() const { return (MoreIntegerType{0b1} << mant_size_m) - 1; };
    IntegerType value_mask() const { return (MoreIntegerType{0b1} << (mant_size_m + exp_size_m + 1)) - 1; };
    IntegerType data_mask() const { return (MoreIntegerType{0b1} << (mant_size_m + exp_size_m + 2)) - 1; };

    IntegerType get_exp() const { return (data_m & exp_mask()) >> mant_size_m; }

    int get_exp_shift() const { return -(0b1 << (exp_size_m - 1)) + 1; };
    IntegerType get_nan_data() const { return exp_mask() | (IntegerType{0b1} << (mant_size_m - 1)) | (IntegerType{0b1} << (mant_size_m + exp_size_m)); };

    int get_exp_value() const { 
        int exp_value = get_exp();
        exp_value += get_exp_shift();
        return exp_value;
    }


    IntegerType get_mant() const { return data_m & mant_mask(); }
    

    std::pair<IntegerType, int> get_normalize_value() const {
        if (!is_denormalized()) {
            return {get_mant(), get_exp_value()};
        }

        IntegerType mant = get_mant();

        int normalize_offset = 0;
        while (((mant << normalize_offset) & exp_mask()) == 0) {
            ++normalize_offset;
        }

        return {(mant << normalize_offset), -(normalize_offset) + get_exp_shift() + 1};
    }


    std::pair<IntegerType, int> get_normalize() const {
        if (!is_denormalized()) {
            return {get_mant(), 0};
        }

        IntegerType mant = get_mant();

        int normalize_offset = 0;;
        while (((mant << normalize_offset) & exp_mask()) == 0) {
            ++normalize_offset;
        }

        return {(mant << normalize_offset) | (~(mant_mask() | exp_mask()) & data_m), normalize_offset - 1};
    }

    bool is_negative() const { return (data_m & (0b1 << (exp_size_m + mant_size_m))) != 0; }

    bool is_nan() const { return get_exp() == (exp_mask() >> mant_size_m) && get_mant() != IntegerType{0b0}; };
    bool is_inf() const { return get_exp() == (exp_mask() >> mant_size_m) && get_mant() == IntegerType{0b0}; };
    bool is_zero() const { return (data_m & (exp_mask() | mant_mask())) == IntegerType{0}; };
    bool is_denormalized() const { return (data_m & exp_mask()) == 0 && !is_zero(); }; 


  private:
    void Add(const FloatingPointNumber& value);
    void Mult(const FloatingPointNumber& value);
    void Division(const FloatingPointNumber& value);

  public:
    FloatingPointNumber& Mad(const FloatingPointNumber& value1, const FloatingPointNumber& value2) { 
        *this *= value1;
        std::cout << output_labwork_task() << std::endl;
        *this += value2;
        return *this;
    };

    FloatingPointNumber& Fma(const FloatingPointNumber& value1, const FloatingPointNumber& value2);

  private:
    MajorIntegerType ShiftLeft(int exp_diff, MajorIntegerType mantisa) const {
        if (!is_denormalized()) {
            mantisa |= mant_mask() + 1;
        }

        return mantisa << exp_diff;
    }

    std::string to_string() const;

    void Negate() { 
        data_m ^= ~(mant_mask() | exp_mask());
        data_m &= value_mask();
    };

  public:
    FloatingPointNumber operator-() const {
        FloatingPointNumber negative_value = *this;
        negative_value.Negate();
        return negative_value;
    };

    FloatingPointNumber& operator+=(const FloatingPointNumber& value) {
        Add(value);
        return *this;
    };
    
    FloatingPointNumber operator+(const FloatingPointNumber& value) const {
        FloatingPointNumber current_value = *this;
        current_value += value;
        return current_value;
    };    
    
    FloatingPointNumber& operator-=(const FloatingPointNumber& value) {
        Add(-value);
        return *this;
    };
    
    FloatingPointNumber operator-(const FloatingPointNumber& value) const {
        FloatingPointNumber current_value = *this;
        current_value -= value;
        return current_value;
    };
    
    FloatingPointNumber& operator*=(const FloatingPointNumber& value) {
        Mult(value);
        return *this;
    };
    
    FloatingPointNumber operator*(const FloatingPointNumber& value) const {
        FloatingPointNumber current_value = *this;
        current_value *= value;
        return current_value;
    };
    
    FloatingPointNumber& operator/=(const FloatingPointNumber& value) {
        Division(value);
        return *this;
    };
    
    FloatingPointNumber operator/(const FloatingPointNumber& value) const {
        FloatingPointNumber current_value = *this;
        current_value /= value;
        return current_value;
    };
    

  public:
    std::string output_labwork_task() const {
        std::stringstream result_ss;

        result_ss << to_string();

        result_ss << " ";

        std::string current_hex = get_hex();
        result_ss << "0x";
        result_ss << std::setw(current_hex.size() - 2 + ((current_hex.size() - 2) % 4 ? 4 - (current_hex.size() - 2) % 4 : 0))
            << std::setfill('0') << std::string_view(current_hex.begin() + 2, current_hex.end());

        return result_ss.str();
    };

    std::string get_hex() const {
        std::stringstream result_ss;

        int hex_size = mant_size_m == 10 ? 4 : 8;
        result_ss << "0x" << std::setw(hex_size) << std::setfill('0') << std::hex << std::uppercase << data_m;
        return result_ss.str();
    }

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

    auto [mantisa, exponent] = get_normalize_value();

    mantisa &= mant_mask();
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


template<RoundingType kRounding>
void FloatingPointNumber<kRounding>::Add(const FloatingPointNumber& value) {
    if (is_nan()) {
        return;
    }

    if (value.is_nan()) {
        data_m = value.data_m;
        return;
    }

    if (!is_inf() && value.is_inf()) {
        data_m = value.data_m;
        return;
    }
    
    if (is_inf() && !value.is_inf()) {
        return;
    }

    if ((is_inf() && value.is_inf()) && !(is_negative() ^ value.is_negative())) {
        return;
    }


    std::array<bool, 2> signs = {is_negative(), value.is_negative()};
    std::array<int, 2> exps = {get_normalize_value().second, value.get_normalize_value().second}; 
    std::array<MajorIntegerType, 2> align_values = {get_normalize_value().first, value.get_normalize_value().first};

    int exp_diff = exps[0] - exps[1];
    if (exp_diff > 0) {
        align_values[0] = is_zero() ? MajorIntegerType{0} : ShiftLeft(exp_diff, align_values[0]);
        align_values[1] = value.is_zero() ? MajorIntegerType{0} : value.ShiftLeft(0, align_values[1]);
    } else {
        align_values[0] = is_zero() ? MajorIntegerType{0} : ShiftLeft(0, align_values[0]);
        align_values[1] = value.is_zero() ? MajorIntegerType{0} : value.ShiftLeft(-exp_diff, align_values[1]);
    }


    MajorIntegerType result_value;
    bool result_sign;
    int result_exp_value = exps[0] < exps[1] ? exps[0] : exps[1];

    if (!(signs[0] ^ signs[1])) {
        result_value = align_values[0] + align_values[1];
        result_sign = signs[0];
    } else {
        result_value = align_values[0] > align_values[1] ? align_values[0] - align_values[1] : align_values[1] - align_values[0];
        result_sign = align_values[0] > align_values[1] ? signs[0] : signs[1];
    }
    if (align_values[0] == align_values[1]) {
        result_sign = false;
    }

    if (result_value == MajorIntegerType{0}) {
        data_m = 0b0;
        // if (signs[0] != result_sign){
        //     Negate();
        // }
        return;
    }


    std::function<std::pair<IntegerType, int> (MajorIntegerType value, MajorIntegerType prev_value)>
        round_value = [&](MajorIntegerType value, MajorIntegerType prev_value)
            -> std::pair<IntegerType, int> {
        int major_bit_pos = bit_count_v<MajorIntegerType> - 1;

        while((value & (MajorIntegerType{0b1} << major_bit_pos)) == MajorIntegerType{0b0}) {
            --major_bit_pos;
        }

        if (major_bit_pos - static_cast<int>(mant_size_m) > 0) {
            if (prev_value == value) {
                return {static_cast<uint64_t>(value >> (major_bit_pos - mant_size_m)), major_bit_pos};
            }

            return round_value(this->rounding(value, major_bit_pos - static_cast<int>(mant_size_m), result_sign), value);

        } else if (major_bit_pos - static_cast<int>(mant_size_m) < 0) {
            return {value << -(major_bit_pos - static_cast<int>(mant_size_m)), major_bit_pos};
        } else {
            return {static_cast<uint64_t>(value), major_bit_pos};
        }
    };


    auto [result_mantisa, major_bit_pos] = round_value(result_value, 0);

    result_mantisa &= mant_mask();
    int result_exp = result_exp_value + (major_bit_pos - mant_size_m);
    result_exp += -get_exp_shift();

    if (result_exp <= 0) {
        result_mantisa |= IntegerType{0b1} << mant_size_m;
        result_mantisa = static_cast<MajorIntegerType>(result_mantisa) >> (-result_exp + 1);
        result_exp = 0;
    } else if (result_exp >= (exp_mask() >> mant_size_m)) {
        auto [upperbound_mant, upperbound_exp] = this->upperbound_overflow(mant_size_m, exp_size_m, result_sign);

        result_exp = static_cast<uint64_t>(upperbound_exp);
        result_mantisa = static_cast<uint64_t>(upperbound_mant);
    } else {
        result_exp <<= mant_size_m;
        result_exp &= exp_mask();
    }

    data_m = (result_mantisa) | (result_exp) | ((result_sign ? IntegerType{0b1} : IntegerType{0b0}) << (mant_size_m + exp_size_m)); 
}


template<RoundingType kRounding>
void FloatingPointNumber<kRounding>::Mult(const FloatingPointNumber& value) {
    if (is_nan()) {
        return;
    }

    if (value.is_nan()) {
        data_m = value.data_m;
        return;
    }

    if ((is_inf() && !value.is_zero() && !value.is_nan()) || (!is_zero() && is_nan() && value.is_inf())) {
        if (is_negative() ^ value.is_negative()) {
            Negate();
        }
        return;
    }

    if ((is_inf() && value.is_zero()) || (is_zero() && value.is_inf())) {
        data_m = get_nan_data();
        return;
    }


    std::array<bool, 2> signs = {is_negative(), value.is_negative()};
    std::array<int, 2> exps = {get_normalize_value().second, value.get_normalize_value().second}; 
    std::array<MajorIntegerType, 2> align_values = {get_normalize_value().first, value.get_normalize_value().first};

    align_values[0] = is_zero() ? MajorIntegerType{0} : ShiftLeft(0, align_values[0]);
    align_values[1] = value.is_zero() ? MajorIntegerType{0} : value.ShiftLeft(0, align_values[1]);


    MajorIntegerType result_value = align_values[0] * align_values[1];
    bool result_sign = signs[0] ^ signs[1];
    int result_exp_value = exps[0] + exps[1];


    if (result_value == MajorIntegerType{0}) {
        data_m = 0b0;
        if (signs[0] != result_sign){
            Negate();
        }
        return;
    }

    std::function<std::pair<IntegerType, int> (MajorIntegerType value, MajorIntegerType prev_value)>
        round_value = [&](MajorIntegerType value, MajorIntegerType prev_value)
        -> std::pair<IntegerType, int> {
    int major_bit_pos = bit_count_v<MajorIntegerType> - 1;

    while((value & (MajorIntegerType{0b1} << major_bit_pos)) == MajorIntegerType{0b0}) {
        --major_bit_pos;
    }

    if (major_bit_pos - static_cast<int>(mant_size_m) > 0) {
        if (prev_value == value) {
            return {static_cast<uint64_t>(value >> (major_bit_pos - mant_size_m)), major_bit_pos};
        }

        int denorm_offset = 0;
        int denorm_detect =  result_exp_value + (major_bit_pos - 2 * mant_size_m) + -get_exp_shift(); 
        if (denorm_detect <= 0)
            denorm_offset = denorm_detect - 1;
        if (-denorm_offset + 1 >= mant_size_m)
            denorm_offset = 0;


        return round_value(this->rounding(value, major_bit_pos - static_cast<int>(mant_size_m) + -denorm_offset, result_sign), value);

    } else if (major_bit_pos - static_cast<int>(mant_size_m) < 0) {
        return {value << -(major_bit_pos - static_cast<int>(mant_size_m)), major_bit_pos};
    } else {
        return {static_cast<uint64_t>(value), major_bit_pos};
    }
};


auto [result_mantisa, major_bit_pos] = round_value(result_value, 0);

int result_exp = result_exp_value + (major_bit_pos - 2 * mant_size_m);
result_exp += -get_exp_shift();

if (-result_exp <= mant_size_m && result_exp <= 0) {
    result_mantisa = this->rounding(result_mantisa, -result_exp + 1, result_sign);
    result_mantisa = result_mantisa >> (-result_exp + 1);
    result_mantisa &= mant_mask();
    result_exp = 0;
    result_exp &= exp_mask();
} else if (result_exp <= 0 && -result_exp + 1 >= mant_size_m) {
    auto [lowerbound_mant, lowerbound_exp] = this->lowerbound_underflow(mant_size_m, -result_exp + 1, result_sign,
        result_mantisa & mant_mask(), mant_size_m + 1);

    result_exp = static_cast<uint64_t>(lowerbound_exp);
    result_mantisa = static_cast<uint64_t>(lowerbound_mant);
} else if (result_exp >= (exp_mask() >> mant_size_m)) {
    auto [upperbound_mant, upperbound_exp] = this->upperbound_overflow(mant_size_m, exp_size_m, result_sign);

    result_exp = static_cast<uint64_t>(upperbound_exp);
    result_mantisa = static_cast<uint64_t>(upperbound_mant);
} else {
    result_exp <<= mant_size_m;
    result_exp &= exp_mask();
}
result_mantisa &= mant_mask();

data_m = (result_mantisa) | (result_exp) | ((result_sign ? IntegerType{0b1} : IntegerType{0b0}) << (mant_size_m + exp_size_m)); 
}

template<RoundingType kRounding>
void FloatingPointNumber<kRounding>::Division(const FloatingPointNumber& value) {
    if (is_nan()) {
        return;
    }

    if (value.is_nan()) {
        data_m = value.data_m;
        return;
    }

    if (!is_inf() && !is_zero() && value.is_inf()) {
        data_m = (is_negative() ^ value.is_negative() ? IntegerType{0b1} : IntegerType{0b0}) << (mant_size_m + exp_size_m);
        return;
    }

    if (is_inf() && !value.is_zero() && !value.is_inf()) {
        data_m = exp_mask() | ((is_negative() ^ value.is_negative() ? IntegerType{0b1} : IntegerType{0b0}) << (mant_size_m + exp_size_m));
        return;
    }

    if (!is_zero() && !is_nan() && value.is_zero()) {
        data_m = exp_mask() | ((is_negative() ^ value.is_negative() ? IntegerType{0b1} : IntegerType{0b0}) << (mant_size_m + exp_size_m));
        return;
    }

    if (is_zero() && !value.is_nan() && !value.is_inf()) {
        data_m = (is_negative() ^ value.is_negative() ? IntegerType{0b1} : IntegerType{0b0}) << (mant_size_m + exp_size_m);
        return;
    }

    if ((is_inf() && value.is_inf()) || (is_zero() && value.is_zero())) {
        data_m = get_nan_data();
        return;
    }


    std::array<bool, 2> signs = {is_negative(), value.is_negative()};
    std::array<int, 2> exps = {get_normalize_value().second, value.get_normalize_value().second}; 
    std::array<MajorIntegerType, 2> align_values = {get_normalize_value().first, value.get_normalize_value().first};

    int dividend_offset = bit_count_v<MajorIntegerType> - (mant_size_m + 1); 
    align_values[0] = is_zero() ? MajorIntegerType{0} : ShiftLeft(dividend_offset, align_values[0]);
    align_values[1] = value.is_zero() ? MajorIntegerType{0} : value.ShiftLeft(0, align_values[1]);


    bool result_sign = signs[0] ^ signs[1];
    int result_exp_value = exps[0] - exps[1];
    
    MajorIntegerType result_value = align_values[0] / align_values[1];

    if (result_value == MajorIntegerType{0}) {
        data_m = 0b0;
        if (signs[0] != result_sign){
            Negate();
        }
        return;
    }


    std::function<std::pair<IntegerType, int> (MajorIntegerType value, MajorIntegerType prev_value)>
        round_value = [&](MajorIntegerType value, MajorIntegerType prev_value)
            -> std::pair<IntegerType, int> {
        int major_bit_pos = bit_count_v<MajorIntegerType> - 1;

        while((value & (MajorIntegerType{0b1} << major_bit_pos)) == MajorIntegerType{0b0}) {
            --major_bit_pos;
        }

        if (major_bit_pos - static_cast<int>(mant_size_m) > 0) {
            if (prev_value == value) {
                return {static_cast<uint64_t>(value >> (major_bit_pos - mant_size_m)), major_bit_pos};
            }

            int denorm_offset = 0;
            int denorm_detect = result_exp_value + (major_bit_pos - dividend_offset) - get_exp_shift();
            if (denorm_detect <= 0) {
                denorm_offset = denorm_detect - 1;
            }
            if (-denorm_offset + 1 >= mant_size_m)
                denorm_offset = 0;

            return round_value(this->rounding(value, major_bit_pos - static_cast<int>(mant_size_m) + -denorm_offset, result_sign), value);

        } else if (major_bit_pos - static_cast<int>(mant_size_m) < 0) {
            return {value << -(major_bit_pos - static_cast<int>(mant_size_m)), major_bit_pos};
        } else {
            return {static_cast<uint64_t>(value), major_bit_pos};
        }
    };


    auto [result_mantisa, major_bit_pos] = round_value(result_value, 0);

    int result_exp = result_exp_value + (major_bit_pos - dividend_offset);
    result_exp += -get_exp_shift();

    if (-result_exp <= mant_size_m && result_exp <= 0) {
        result_mantisa = this->rounding(result_mantisa, -result_exp + 1, result_sign);

        result_mantisa = result_mantisa >> (-result_exp + 1);
        result_mantisa &= mant_mask();
        result_exp = 0;
        result_exp &= exp_mask();
    } else if (result_exp <= 0 && -result_exp + 1 >= mant_size_m) {
        auto [lowerbound_mant, lowerbound_exp] = this->lowerbound_underflow(mant_size_m, -result_exp + 1, result_sign,
            result_mantisa & mant_mask(), mant_size_m + 1);

        result_exp = static_cast<uint64_t>(lowerbound_exp);
        result_mantisa = static_cast<uint64_t>(lowerbound_mant);
    } else if (result_exp >= (exp_mask() >> mant_size_m)) {
        auto [upperbound_mant, upperbound_exp] = this->upperbound_overflow(mant_size_m, exp_size_m, result_sign);

        result_exp = static_cast<uint64_t>(upperbound_exp);
        result_mantisa = static_cast<uint64_t>(upperbound_mant);
    } else {
        result_exp <<= mant_size_m;
        result_exp &= exp_mask();
    }
    result_mantisa &= mant_mask();

    data_m = (result_mantisa) | (result_exp) | ((result_sign ? IntegerType{0b1} : IntegerType{0b0}) << (mant_size_m + exp_size_m)); 
}


#if 0
template<RoundingType kRounding>
FloatingPointNumber<kRounding>& FloatingPointNumber<kRounding>::Fma(const FloatingPointNumber& value1, const FloatingPointNumber& value3) {
    FloatingPointNumber value2 = value3;

    std::array<bool, 2> signs = {is_negative(), value1.is_negative()};
    std::array<int, 2> exps = {get_normalize_value().second, value1.get_normalize_value().second}; 
    std::array<MajorIntegerType, 2> align_values = {get_normalize_value().first, value1.get_normalize_value().first};

    align_values[0] = is_zero() ? MajorIntegerType{0} : ShiftLeft(0, align_values[0]);
    align_values[1] = value1.is_zero() ? MajorIntegerType{0} : value1.ShiftLeft(0, align_values[1]);


    MajorIntegerType result_value = align_values[0] * align_values[1];
    bool result_sign = signs[0] ^ signs[1];
    int result_exp_value = exps[0] + exps[1];

    FloatingPointNumber fst_op_exstra_case = *this;
    fst_op_exstra_case.data_m = 0;

    if (is_nan()) {
        fst_op_exstra_case = *this;


    } else if (value1.is_nan()) {
        fst_op_exstra_case.data_m = value1.data_m;


    } else if ((is_inf() && !value1.is_zero() && !value1.is_nan()) || (!is_zero() && is_nan() && value1.is_inf())) {
        fst_op_exstra_case = *this;
        if (is_negative() ^ value1.is_negative()) {
            fst_op_exstra_case.Negate();
        }


    } else if ((is_inf() && value1.is_zero()) || (is_zero() && value1.is_inf())) {
        fst_op_exstra_case.data_m = get_nan_data();


    } else if (result_value == MajorIntegerType{0}) {
        fst_op_exstra_case = *this;
        fst_op_exstra_case.data_m = 0b0;

        if (signs[0] != result_sign){
            fst_op_exstra_case.Negate();
        }
    }

    // summ begin


#if 1
    if (fst_op_exstra_case.is_nan()) {
        return *this;
    }

    if (value2.is_nan()) {
        data_m = value2.data_m;
        return *this;
    }

    if (!fst_op_exstra_case.is_inf() && value2.is_inf()) {
        data_m = value2.data_m;
        return *this;
    }
    
    if (fst_op_exstra_case.is_inf() && !value2.is_inf()) {
        data_m = fst_op_exstra_case.data_m;
        return *this;
    }

    if ((fst_op_exstra_case.is_inf() && value2.is_inf()) && !(fst_op_exstra_case.is_negative() ^ value2.is_negative())) {
        data_m = fst_op_exstra_case.data_m;
        return *this;
    }
#endif
    // result_exp_value += mant_size_m;

    std::array<bool, 2> signs_summ = {result_sign, value2.is_negative()};
    std::array<int, 2> exps_summ = {result_exp_value, value2.get_normalize_value().second}; 
    std::array<MajorIntegerType, 2> align_values_summ = {result_value, value2.get_normalize_value().first};

    int exp_diff_summ = exps_summ[0] - exps_summ[1];
    if (exp_diff_summ > 0) {
        align_values_summ[0] <<= exp_diff_summ;
        align_values_summ[1] = value2.is_zero() ? MajorIntegerType{0} : value2.ShiftLeft(0, align_values_summ[1]);
    } else {
        align_values_summ[1] = value2.is_zero() ? MajorIntegerType{0} : value2.ShiftLeft(-exp_diff_summ, align_values_summ[1]);
    }


    MajorIntegerType result_value_summ;
    bool result_sign_summ;
    int result_exp_value_summ = exps_summ[0] < exps_summ[1] ? exps_summ[0] : exps_summ[1];

    if (!(signs_summ[0] ^ signs_summ[1])) {
        result_value_summ = align_values_summ[0] + align_values_summ[1];
        result_sign_summ = signs_summ[0];
    } else {
        result_value_summ = align_values_summ[0] > align_values_summ[1] ? align_values_summ[0] - align_values_summ[1] : align_values_summ[1] - align_values_summ[0];
        result_sign_summ = align_values_summ[0] > align_values_summ[1] ? signs_summ[0] : signs_summ[1];
    }

    if (result_value_summ == MajorIntegerType{0}) {
        data_m = 0b0;
        if (signs_summ[0] != result_sign_summ){
            Negate();
        }
        return *this;
    }




    std::function<std::pair<IntegerType, int> (MajorIntegerType value, MajorIntegerType prev_value)>
        round_value = [&](MajorIntegerType value, MajorIntegerType prev_value)
            -> std::pair<IntegerType, int> {
        int major_bit_pos = bit_count_v<MajorIntegerType> - 1;

        while((value & (MajorIntegerType{0b1} << major_bit_pos)) == MajorIntegerType{0b0}) {
            --major_bit_pos;
        }

        if (major_bit_pos - static_cast<int>(mant_size_m) > 0) {
            if (prev_value == value) {
                return {static_cast<uint64_t>(value >> (major_bit_pos - mant_size_m)), major_bit_pos};
            }

            return round_value(this->rounding(value, major_bit_pos - static_cast<int>(mant_size_m), result_sign_summ), value);

        } else if (major_bit_pos - static_cast<int>(mant_size_m) < 0) {
            return {value << -(major_bit_pos - static_cast<int>(mant_size_m)), major_bit_pos};
        } else {
            return {static_cast<uint64_t>(value), major_bit_pos};
        }
    };


    auto [result_mantisa_summ, major_bit_pos_summ] = round_value(result_value_summ, 0);

    result_mantisa_summ &= mant_mask();
    int result_exp_summ = result_exp_value_summ + (major_bit_pos_summ - mant_size_m);
    result_exp_summ += -get_exp_shift();

    if (result_exp_summ <= 0) {
        result_mantisa_summ |= IntegerType{0b1} << mant_size_m;
        result_mantisa_summ = static_cast<MajorIntegerType>(result_mantisa_summ) >> (-result_exp_summ + 1);
        result_exp_summ = 0;
    } else if (result_exp_summ >= (exp_mask() >> mant_size_m)) {
        auto [upperbound_mant, upperbound_exp] = this->upperbound_overflow(mant_size_m, exp_size_m, result_sign_summ);

        result_exp_summ = static_cast<uint64_t>(upperbound_exp);
        result_mantisa_summ = static_cast<uint64_t>(upperbound_mant);
    } else {
        result_exp_summ <<= mant_size_m;
        result_exp_summ &= exp_mask();
    }

    data_m = (result_mantisa_summ) | (result_exp_summ) | ((result_sign_summ ? IntegerType{0b1} : IntegerType{0b0}) << (mant_size_m + exp_size_m)); 

    return *this;
};
#else
template<RoundingType kRounding>
FloatingPointNumber<kRounding>& FloatingPointNumber<kRounding>::Fma(const FloatingPointNumber& value1, const FloatingPointNumber& value2) {
    return Mad(value1, value2);
}
#endif // fma try

#endif // _FLOAT_NUMBER_HPP_