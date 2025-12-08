#include <gtest/gtest.h>

#include "scan.hpp"

TEST(ScanTest, SimpleTest) {
    auto result = stdx::scan<std::string>("number"_fs, "{}");
    ASSERT_FALSE(result);
}

TEST(ParseInputTest, SingleValueSuccess) {
    stdx::format_string<> input = "{%d}"_fs;
    stdx::details::fixed_string<> values("123");

    auto result = stdx::details::parse_input<int>(input, values);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(std::get<0>(*result), 123);
}

TEST(ParseInputTest, WrongType) {
    stdx::format_string<> input = "{%f}"_fs;
    stdx::details::fixed_string<> values("123");

    auto result = stdx::details::parse_input<int>(input, values);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().message, "Expected %d format and non-empty input");
}

TEST(ParseInputTest, MultipleValuesSuccess) {
    auto result = stdx::scan<int, double>("{%d} {%f}"_fs, "42 3.14");
    ASSERT_TRUE(result.has_value());
    auto [i, d] = *result;
    std::cout << i << " " << d;
    EXPECT_EQ(i, 42);
    EXPECT_DOUBLE_EQ(d, 3.14);
}

TEST(ParseInputTest, WrongPlacholder) {
    auto result = stdx::scan<int, double>("{%d} {%f}"_fs, "3.14 42");
    EXPECT_EQ(result.error().message, "Integer parse failed or incomplete");
}

TEST(ParseInputTest, MultipleIntDoubleStringSuccess) {
    auto result = stdx::scan<int, double, std::string_view>("{%d} {%f} {%s}"_fs, "42 3.14 world");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(std::get<0>(*result), 42);
    EXPECT_DOUBLE_EQ(std::get<1>(*result), 3.14);
    EXPECT_EQ(std::get<2>(*result), "world");
}

TEST(ParseInputTest, SingleStringSuccess) {
    auto result = stdx::scan<std::string_view>("{%s}"_fs, "hello");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(std::get<0>(*result), "hello");
}


TEST(ParseInputTest, ExtraValuesIgnored) {
    auto result = stdx::scan<int>("{%d}"_fs, "42 extra");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(std::get<0>(*result), 42);
}

TEST(ParseInputTest, NoPlaceholdersError) {
    auto result = stdx::scan<int>("hello"_fs, "42");
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().message, "Wrong format specifier");
}

TEST(ParseInputTest, Int16Type) {
    auto result = stdx::scan<int16_t>("{%d}"_fs, "42 extra");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(std::get<0>(*result), 42);
}


TEST(ParseInputTest, UnsignedType) {
    auto result = stdx::scan<uint8_t>("{%u}"_fs, "42 extra");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(std::get<0>(*result), 42);
}

// При неверном типе не компилируется
// struct Integer { int i = 0; };

// TEST(ParseInputTest, DifferentPlaceHolder) {
//     auto result = stdx::scan<Integer>("{%d}"_fs, "42");
//     EXPECT_EQ(result.error().message, "Expected %d format and non-empty input");
// }