#include <cstddef>
#include <iostream>
#include <cstdio>
#include <string_view>
#include <sstream>
#include <string>
#include <vector>

#include "float_number.hpp"
#include "rounding_device.hpp"

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
        int operation_index = 3;
        if (!std::sscanf(argv[4], "%x", &data_fst) || !std::sscanf(argv[5], "%x", &data_scd)) {
            std::cout << "parse data error" << "\n";
            return 0;
        }

        auto do_action  = [](auto& fst, auto& scd, std::string_view op) {
            if (op == "+") {
                std::cout << (fst + scd).output_labwork_task();
            } else if (op == "-") {
                std::cout << (fst - scd).output_labwork_task();
            } else if (op == "*" || op == "M") {
                std::cout << (fst * scd).output_labwork_task();
            } else if (op == "/") {
                std::cout << (fst / scd).output_labwork_task();
            }
        };

        if (std::string_view{argv[2]} == "0") {
            FloatingPointNumber<RoundingType::TOWARD_ZERO> number_fst{argv[1], data_fst};
            FloatingPointNumber<RoundingType::TOWARD_ZERO> number_scd{argv[1], data_scd};
            do_action(number_fst, number_scd, argv[operation_index]);

        } else if (std::string_view{argv[2]} == "1") {
            FloatingPointNumber<RoundingType::TOWARD_EVEN> number_fst{argv[1], data_fst};
            FloatingPointNumber<RoundingType::TOWARD_EVEN> number_scd{argv[1], data_scd};
            do_action(number_fst, number_scd, argv[operation_index]);

        } else if (std::string_view{argv[2]} == "2") {
            FloatingPointNumber<RoundingType::TOWARD_POS_INF> number_fst{argv[1], data_fst};
            FloatingPointNumber<RoundingType::TOWARD_POS_INF> number_scd{argv[1], data_scd};
            do_action(number_fst, number_scd, argv[operation_index]);

        } else if (std::string_view{argv[2]} == "3") {
            FloatingPointNumber<RoundingType::TOWARD_NEG_INF> number_fst{argv[1], data_fst};
            FloatingPointNumber<RoundingType::TOWARD_NEG_INF> number_scd{argv[1], data_scd};
            do_action(number_fst, number_scd, argv[operation_index]);

        } else {
            std::cout << "unknowing rounding type" << "\n";
            return 0;
        }
    }


    return 0;
};

int test_function() {
#if 0
    FloatingPointNumber<RoundingType::TOWARD_EVEN> fst{"h", 0xb736};
    FloatingPointNumber<RoundingType::TOWARD_EVEN> scd{"h", 0x7a86};

    std::cout << fst.output_labwork_task() << " / ";
    std::cout << fst.get_hex() << " ";
    std::cout << "\n";
    std::cout << " / ";
    std::cout << "\n";
    std::cout << scd.output_labwork_task() << " / ";
    std::cout << scd.get_hex() << " ";
    std::cout << "\n";
    std::cout << " = ";
    
    std::cout << "\n";
    std::cout << (fst / scd).get_hex();
    std::cout << "\n";
    std::cout << (fst / scd).output_labwork_task();
    std::cout << "\n";
    return 0;

    // Ожидалось: '-0x1.1a0p-17'
    // Получено:  '-0x1.1c0p-17'

  #else

    std::stringstream input_ss;

    input_ss << ". ";
    input_ss << "h 3 + 0xFF10 0x7F01";



    int argc_test = 6;
    std::vector<std::string> input_cont(argc_test);
    char* argv_test[6];
    for (auto& arg : input_cont) {
        input_ss >> arg;
    }
    for (size_t i = 0; i < argc_test; ++i) {
        argv_test[i] = input_cont[i].data();
    }
  
    return labwork_tests_parser(argc_test, argv_test);
#endif
}

int main(int argc, char** argv) {
    return labwork_tests_parser(argc, argv);
};