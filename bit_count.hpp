#ifndef _BIT_COUNT_HPP_
#define _BIT_COUNT_HPP_

#include <cstddef>

template<typename IntegerType>
struct bit_count {
    static constexpr size_t value = sizeof(IntegerType) << 3;
};

template<typename IntegerType>
static constexpr size_t bit_count_v = bit_count<IntegerType>::value;

#endif // _BIT_COUNT_HPP_