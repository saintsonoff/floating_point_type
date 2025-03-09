#include <cstddef>
#include <iostream>
#include <cstdio>
#include <string_view>
#include <sstream>
#include <string>
#include <vector>

#include "bit_count.hpp"
#include "float_number.hpp"
#include "rounding_device.hpp"
#include "very_long_int.hpp"

int labwork_tests_parser(int argc, char** argv) {
    if (argc != 4 && argc != 6 && argc != 7) {
        std::cout << "incorrect params count" << "\n";
        return 0;
    }

    if (argc == 4) {
        uint32_t data;
        if (!std::sscanf(argv[3], "%x", &data)) {
            std::cout << "parse data error" << "\n";
            return 0;
        }

        auto do_action  = [](auto& value) {
            std::cout << value.output_labwork_task();
            // std::cout << value;
        };

        if (std::string_view{argv[2]} == "0") {
            FloatingPointNumber<RoundingType::TOWARD_ZERO> number{argv[1], data};
            do_action(number);

        } else if (std::string_view{argv[2]} == "1") {
            FloatingPointNumber<RoundingType::TOWARD_EVEN> number{argv[1], data};
            do_action(number);
            
        } else if (std::string_view{argv[2]} == "2") {
            FloatingPointNumber<RoundingType::TOWARD_POS_INF> number{argv[1], data};
            do_action(number);

        } else if (std::string_view{argv[2]} == "3") {
            FloatingPointNumber<RoundingType::TOWARD_NEG_INF> number{argv[1], data};
            do_action(number);

        } else {
            std::cout << "unknowing rounding type" << "\n";
            return 0;
        }
    }


    if (argc == 6) {
        uint32_t data_fst, data_scd;
        if (!std::sscanf(argv[3], "%x", &data_fst) || !std::sscanf(argv[5], "%x", &data_scd)) {
            std::cout << "parse data error" << "\n";
            return 0;
        }

        auto do_action  = [](auto& fst, auto& scd, std::string_view op) {
            if (op == "+") {
                std::cout << (fst + scd).output_labwork_task();
            } else if (op == "-") {
                std::cout << (fst - scd).output_labwork_task();
            } else if (op == "*") {
                // std::cout << (fst * scd).output_labwork_task();
            } else if (op == "/") {
                // std::cout << (fst / scd).output_labwork_task();
            }
        };

        if (std::string_view{argv[2]} == "0") {
            FloatingPointNumber<RoundingType::TOWARD_ZERO> number_fst{argv[1], data_fst};
            FloatingPointNumber<RoundingType::TOWARD_ZERO> number_scd{argv[1], data_scd};
            do_action(number_fst, number_scd, argv[4]);

        } else if (std::string_view{argv[2]} == "1") {
            FloatingPointNumber<RoundingType::TOWARD_EVEN> number_fst{argv[1], data_fst};
            FloatingPointNumber<RoundingType::TOWARD_EVEN> number_scd{argv[1], data_scd};
            do_action(number_fst, number_scd, argv[4]);

        } else if (std::string_view{argv[2]} == "2") {
            FloatingPointNumber<RoundingType::TOWARD_POS_INF> number_fst{argv[1], data_fst};
            FloatingPointNumber<RoundingType::TOWARD_POS_INF> number_scd{argv[1], data_scd};
            do_action(number_fst, number_scd, argv[4]);

        } else if (std::string_view{argv[2]} == "3") {
            FloatingPointNumber<RoundingType::TOWARD_NEG_INF> number_fst{argv[1], data_fst};
            FloatingPointNumber<RoundingType::TOWARD_NEG_INF> number_scd{argv[1], data_scd};
            do_action(number_fst, number_scd, argv[4]);

        } else {
            std::cout << "unknowing rounding type" << "\n";
            return 0;
        }
    }


    return 0;
};

int main(int argc, char** argv) {
    return labwork_tests_parser(argc, argv);
};