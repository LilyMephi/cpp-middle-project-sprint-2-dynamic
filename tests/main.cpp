#include "parse.hpp"
#include "format_string.hpp"

#include "scan.hpp"
// #include <gtest/gtest.h>
#include <iostream>
int main(int argc, char **argv) {
    int num;
    double flt;
    std::string_view str;

    // "42" -> %d -> 42
    stdx::details::parse_value("42", "%d", num);  // num = 42 [web:6]
    std::cout << num << std::endl;
    // "3.14" -> %f -> 3.14
    stdx::details::parse_value("3.14", "%f", flt);  // flt = 3.14
    std::cout << flt << std::endl;
    // "hello" -> %s -> "hello"
    stdx::details::parse_value("hello", "%s", str);  // str = "hello"
    std::cout << str << std::endl;
    auto res =  stdx::format_string<>::get_number_placeholders("i have {%f} apple and my dad has {%s} apple sum is {%d}");

    if (res) {
        std::cout << "Успех: " << *res << std::endl;  // значение
    } else {
        std::cout << "Ошибка: " << res.error().message << std::endl;  // ошибка
    }
    auto fmt1 = "i have {%d} apples and rubels{%s}"_fs;
    for (auto [pos, ph] : fmt1.placeholder_positions) {
        std::cout << "Pos: " << pos << ", PH: " << ph << '\n';
    }
    return 0;
    // ::testing::InitGoogleTest(&argc, argv);
    // return RUN_ALL_TESTS();
}
