#ifndef _ROUNDING_DEVICE_HPP_
#define _ROUNDING_DEVICE_HPP_

#include <cstdint>
#include <cstddef>
#include <utility>

#include <string>
#include <iostream>

enum RoundingType { TOWARD_ZERO, TOWARD_EVEN, TOWARD_POS_INF, TOWARD_NEG_INF };

template<RoundingType kRounding, typename IntegerType = uint64_t>
class RoundingDevice {  };


template<typename IntegerType>
class RoundingDevice<RoundingType::TOWARD_EVEN, IntegerType> {
  protected:
    IntegerType rounding(IntegerType value, int rounded_digit_pos, bool is_negative) {
		IntegerType rounded_mask = (IntegerType{0b1} << rounded_digit_pos) - IntegerType{0b1};

		IntegerType half_of_major_digit = IntegerType{0b1} << rounded_digit_pos - 1;
		IntegerType result_value = value & ~rounded_mask;
		IntegerType rounded_value = value & rounded_mask;

		if (rounded_value > half_of_major_digit) {
			result_value += IntegerType{0b1} << rounded_digit_pos;
		} else if (rounded_value == half_of_major_digit) {
			if (((IntegerType{0b1} << rounded_digit_pos) & value) != IntegerType{0b0}) {
				result_value += IntegerType{0b1} << rounded_digit_pos;
			} else {
				result_value = value & ~rounded_mask;
			}

		} else {
			result_value = value & ~rounded_mask;
		}

		return result_value;
    };

	std::pair<IntegerType, IntegerType> upperbound_overflow(IntegerType mant_size, IntegerType exp_size, bool is_negative) {
		std::pair<IntegerType, IntegerType> inf = {0, ((IntegerType{0b1} << exp_size) - IntegerType{0b1})  << mant_size};

		return inf;
	}

	std::pair<IntegerType, IntegerType> lowerbound_underflow(IntegerType mant_size, IntegerType exp_size, bool is_negative,
      IntegerType value, int rounded_digit_pos) {

		IntegerType rounded_mask = (IntegerType{0b1} << rounded_digit_pos) - IntegerType{0b1};

		IntegerType half_of_major_digit = IntegerType{0b1} << rounded_digit_pos - 1;
		IntegerType rounded_value = value & rounded_mask;

		if (rounded_value > half_of_major_digit) {
			IntegerType min_mant = IntegerType{0b1};
			IntegerType min_exp = 0;
	
			return {min_mant, min_exp};
		}
		
		return {0, 0};
	}
};


template<typename IntegerType>
class RoundingDevice<RoundingType::TOWARD_ZERO, IntegerType> {
  protected:
    IntegerType rounding(IntegerType value, int rounded_digit_pos, bool is_negative) {
		IntegerType rounded_mask = (IntegerType{0b1} << rounded_digit_pos) - IntegerType{0b1};

		IntegerType result_value = value & ~rounded_mask;
		return result_value;
    };

	std::pair<IntegerType, IntegerType> upperbound_overflow(IntegerType mant_size, IntegerType exp_size, bool is_negative) {
		IntegerType max_mant = (IntegerType{0b1} << mant_size) - IntegerType{0b1};
		IntegerType max_exp =  (((IntegerType{0b1} << exp_size) - IntegerType{0b1}) - IntegerType{0b1}) << mant_size;

		return {max_mant, max_exp};
	}

	std::pair<IntegerType, IntegerType> lowerbound_underflow(IntegerType mant_size, IntegerType exp_size, bool is_negative,
			IntegerType , int) {
		return {0, 0};
	}
};


template<typename IntegerType>
class RoundingDevice<RoundingType::TOWARD_POS_INF, IntegerType> {
  protected:
    IntegerType rounding(IntegerType value, int rounded_digit_pos, bool is_negative) {
		IntegerType rounded_mask = (IntegerType{0b1} << rounded_digit_pos) - IntegerType{0b1};

		IntegerType result_value = value & ~rounded_mask;

		if ((value & rounded_mask) != IntegerType{0b0} && !is_negative) {
			result_value += IntegerType{0b1} << rounded_digit_pos;
		}

		return result_value;
    };


	std::pair<IntegerType, IntegerType> upperbound_overflow(IntegerType mant_size, IntegerType exp_size, bool is_negative) {
		IntegerType max_mant = (IntegerType{0b1} << mant_size) - IntegerType{0b1};
		IntegerType max_exp =  (((IntegerType{0b1} << exp_size) - IntegerType{0b1}) - IntegerType{0b1}) << mant_size;

		std::pair<IntegerType, IntegerType> inf = {0, ((IntegerType{0b1} << exp_size) - IntegerType{0b1})  << mant_size};
	
		if (!is_negative) {
			return inf;
		}

		return {max_mant, max_exp};
	}

	std::pair<IntegerType, IntegerType> lowerbound_underflow(IntegerType mant_size, IntegerType exp_size, bool is_negative,
			IntegerType , int) {
		IntegerType min_mant = IntegerType{0b1};
		IntegerType min_exp = 0;

		std::pair<IntegerType, IntegerType> inf = {0, ((IntegerType{0b1} << exp_size) - IntegerType{0b1})  << mant_size};
	
		if (is_negative) {
			return {0, 0};
		}

		return {min_mant, min_exp};
	}
};


template<typename IntegerType>
class RoundingDevice<RoundingType::TOWARD_NEG_INF, IntegerType> {
  protected:
    IntegerType rounding(IntegerType value, int rounded_digit_pos, bool is_negative) {
		IntegerType rounded_mask = (IntegerType{0b1} << rounded_digit_pos) - IntegerType{0b1};

		IntegerType result_value = value & ~rounded_mask;

		if ((value & rounded_mask) != IntegerType{0b0} && is_negative) {
			result_value += IntegerType{0b1} << rounded_digit_pos;
		}

		return result_value;
    };

	std::pair<IntegerType, IntegerType> upperbound_overflow(IntegerType mant_size, IntegerType exp_size, bool is_negative) {
		IntegerType max_mant = (IntegerType{0b1} << mant_size) - IntegerType{0b1};
		IntegerType max_exp =  (((IntegerType{0b1} << exp_size) - IntegerType{0b1}) - IntegerType{0b1}) << mant_size;

		std::pair<IntegerType, IntegerType> inf = {0, ((IntegerType{0b1} << exp_size) - IntegerType{0b1})  << mant_size};
	
		if (is_negative) {
			return inf;
		}

		return {max_mant, max_exp};
	}

	std::pair<IntegerType, IntegerType> lowerbound_underflow(IntegerType mant_size, IntegerType exp_size, bool is_negative,
			IntegerType , int) {
		IntegerType min_mant = IntegerType{0b1};
		IntegerType min_exp = 0;
	
		if (!is_negative) {
			return {0, 0};
		}

		return {min_mant, min_exp};
	}
};

#endif // _ROUNDING_DEVICE_HPP_