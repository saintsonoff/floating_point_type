#include <cstddef>
#include <iostream>
#include <cstdio>
#include <string_view>
#include <sstream>
#include <string>
#include <fstream>
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
        } else {
                std::cout << "uncorrect operation";
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


    if (argc == 7) {
        uint32_t data_fst, data_scd, data_thd;
        int operation_index = 3;
        if (!std::sscanf(argv[4], "%x", &data_fst) || !std::sscanf(argv[5], "%x", &data_scd) || !std::sscanf(argv[6], "%x", &data_thd)) {
            std::cout << "parse data error" << "\n";
            return 0;
        }

        auto do_action  = [](auto& fst, auto& scd, auto& thd, std::string_view op) {
            if (op == "mad") {
                std::cout << fst.Mad(scd, thd).output_labwork_task();
            } else if (op == "fma") {
                std::cout << fst.Fma(scd, thd).output_labwork_task();
            } else {
                std::cout << "uncorrect operation";
            }
        };

        if (std::string_view{argv[2]} == "0") {
            FloatingPointNumber<RoundingType::TOWARD_ZERO> number_fst{argv[1], data_fst};
            FloatingPointNumber<RoundingType::TOWARD_ZERO> number_scd{argv[1], data_scd};
            FloatingPointNumber<RoundingType::TOWARD_ZERO> number_thd{argv[1], data_thd};
            do_action(number_fst, number_scd, number_thd, argv[operation_index]);

        } else if (std::string_view{argv[2]} == "1") {
            FloatingPointNumber<RoundingType::TOWARD_EVEN> number_fst{argv[1], data_fst};
            FloatingPointNumber<RoundingType::TOWARD_EVEN> number_scd{argv[1], data_scd};
            FloatingPointNumber<RoundingType::TOWARD_EVEN> number_thd{argv[1], data_thd};
            do_action(number_fst, number_scd, number_thd, argv[operation_index]);

        } else if (std::string_view{argv[2]} == "2") {
            FloatingPointNumber<RoundingType::TOWARD_POS_INF> number_fst{argv[1], data_fst};
            FloatingPointNumber<RoundingType::TOWARD_POS_INF> number_scd{argv[1], data_scd};
            FloatingPointNumber<RoundingType::TOWARD_POS_INF> number_thd{argv[1], data_thd};
            do_action(number_fst, number_scd, number_thd, argv[operation_index]);

        } else if (std::string_view{argv[2]} == "3") {
            FloatingPointNumber<RoundingType::TOWARD_NEG_INF> number_fst{argv[1], data_fst};
            FloatingPointNumber<RoundingType::TOWARD_NEG_INF> number_scd{argv[1], data_scd};
            FloatingPointNumber<RoundingType::TOWARD_NEG_INF> number_thd{argv[1], data_thd};
            do_action(number_fst, number_scd, number_thd, argv[operation_index]);

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

    input_ss << "s 1 mad 0x3dcccccd 0x41200000 0xbf800000";

    // input_ss << "s 1 / 0x0C8E90AF 0x4C5A48B5";
    // // correct: 0x1.4e6578p-128 0x0029CCAF
    // // uncorrect: 0x1.4e6570p-128 0x0029CCAE


    // input_ss << "s 1 / 0xB4E42D66 0xF437EC43";
    // // correct: 0x1.3d98e4p-127 0x004F6639
    // // uncorrect: 0x1.3d98e0p-127 0x004F6638

    
    // input_ss << "s 1 - 0x718DDE74 0xF3429B8E";
    // // correct: 0x1.a8aeb8p+103 0x7354575C
    // // uncorrect: 0x1.a8aebap+103 0x7354575D
        
    // input_ss << "s 0 fma 0x8560D2CA 0x749958E1 0xA5D1BECE";
    // correct: -0x1.0d5810p-10 0xBA86AC08
    // uncorrect: -0x1.0d5810p+31 0xCF06AC08


    // input_ss << "s 1 / 0x0E9A34A8 0xCE44B6C9";
    // // correct:   -0x1.915c68p-128 0x80322B8D
    // // uncorrect: -0x1.915c60p-128 0x80322B8C
    


    int argc_test = 7;
    std::vector<std::string> input_cont(argc_test);
    char* argv_test[7];
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