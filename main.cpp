#include <iostream>
#include <cstdio>
#include <string_view>

#include "float_number.hpp"

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


    return 0;
};

int main(int argc, char** argv) {
    return labwork_tests_parser(argc, argv);

    int argc_test = 4;
    char* argv_test[] = {
        ".",
        "h", "2", "0x108"
    };

    // 0x1.080p-16
  
    return labwork_tests_parser(argc_test, argv_test);
};