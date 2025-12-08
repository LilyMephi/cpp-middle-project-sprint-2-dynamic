#pragma once

#include <charconv>
#include <expected>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "format_string.hpp"
#include "types.hpp"
#include <iostream>
namespace stdx::details {

template <typename T>
std::expected<void, details::scan_error> parse_value(std::string_view input, std::string_view fmt, T &value) {
    static_assert(sizeof(T) == 0, "parse_value is not implemented for this type T");
    return std::unexpected(details::scan_error{"parse_value not implemented for this type"});
}

template <typename T>
requires std::is_integral_v<T>
    std::expected<void, details::scan_error> parse_value(std::string_view input, std::string_view fmt, T &value) {
    std::cout << fmt; 
    if (fmt.size() != 2 || fmt[0] != '%') {
        return std::unexpected(details::scan_error{"Invalid format"});
    }

    if constexpr (std::is_unsigned_v<T>) {
        if (fmt[1] != 'u')
            return std::unexpected(details::scan_error{"Expected %u format and non-empty input"});
    } else {
        if (fmt[1] != 'd')
            return std::unexpected(details::scan_error{"Expected %d format and non-empty input"});
    }

    auto res = std::from_chars(input.data(), input.data() + input.size(), value);
    if (res.ec != std::errc{} || res.ptr != input.data() + input.size())
        return std::unexpected(details::scan_error{"Integer parse failed or incomplete"});

    return {};
}

template <>
std::expected<void, details::scan_error> parse_value<float>(std::string_view input, std::string_view fmt,
                                                            float &value) {
    if (fmt[1] != 'f')
        return std::unexpected(details::scan_error{"Expected %f for float"});
    auto res = std::from_chars(input.data(), input.data() + input.size(), value);
    if (res.ec != std::errc{})
        return std::unexpected(details::scan_error{"Float parse failed or input not fully consumed"});
    return {};
}

template <>
std::expected<void, details::scan_error> parse_value<double>(std::string_view input, std::string_view fmt,
                                                             double &value) {
    if (fmt[1] != 'f')
        return std::unexpected(details::scan_error{"Expected %f for double"});
    auto res = std::from_chars(input.data(), input.data() + input.size(), value);
    if (res.ec != std::errc{})
        return std::unexpected(details::scan_error{"Double parse failed or input not fully consumed"});
    return {};
}

template <>
std::expected<void, details::scan_error> parse_value<std::string_view>(std::string_view input, std::string_view fmt,
                                                                       std::string_view &value) {
    if (fmt[1] != 's')
        return std::unexpected(details::scan_error{"Expected %s format for string_view"});
    value = input;
    return {};
}

template <>
std::expected<void, details::scan_error> parse_value<std::string>(std::string_view input, std::string_view fmt,
                                                                  std::string &value) {
    if (fmt[1] != 's')
        return std::unexpected(details::scan_error{"Expected %s format for string"});
    value = std::string(input);
    return {};
}

// Функция для парсинга значения с учетом спецификатора формата
template <typename T>
std::expected<T, scan_error> parse_value_with_format(std::string_view input, std::string_view fmt) {
    if (fmt.empty() || fmt.size() < 4) {
        return std::unexpected(details::scan_error{"Wrong format specifier"});
    }
    std::string_view inner = fmt.substr(1, fmt.size() - 2);
    T value;
    auto result = parse_value(input, inner, value);
    if (!result) {
        return std::unexpected(result.error());
    }

    return value;
}

// Функция для проверки корректности входных данных и выделения из обеих строк интересующих данных для парсинга
template <typename... Ts>
std::expected<std::pair<std::vector<std::string_view>, std::vector<std::string_view>>, scan_error>
parse_sources(std::string_view input, std::string_view format) {
    std::vector<std::string_view> format_parts;  // Части формата между {}
    std::vector<std::string_view> input_parts;
    size_t start = 0;
    while (true) {
        size_t open = format.find('{', start);
        if (open == std::string_view::npos) {
            break;
        }
        size_t close = format.find('}', open);
        if (close == std::string_view::npos) {
            break;
        }

        // Если между предыдущей } и текущей { есть текст,
        // проверяем его наличие во входной строке
        if (open > start) {
            std::string_view between = format.substr(start, open - start);
            auto pos = input.find(between);
            if (input.size() < between.size() || pos == std::string_view::npos) {
                return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
            }
            if (start != 0) {
                input_parts.emplace_back(input.substr(0, pos));
            }

            input = input.substr(pos + between.size());
        }

        // Сохраняем спецификатор формата (то, что между {})
        format_parts.push_back(format.substr(open + 1, close - open - 1));
        start = close + 1;
    }

    // Проверяем оставшийся текст после последней }
    if (start < format.size()) {
        std::string_view remaining_format = format.substr(start);
        auto pos = input.find(remaining_format);
        if (input.size() < remaining_format.size() || pos == std::string_view::npos) {
            return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
        }
        input_parts.emplace_back(input.substr(0, pos));
        input = input.substr(pos + remaining_format.size());
    } else {
        input_parts.emplace_back(input);
    }
    return std::pair{format_parts, input_parts};
}

template <typename T, typename... Ts>
std::expected<std::tuple<T, Ts...>, scan_error> parse_input(format_string<> input, details::fixed_string<> values,
                                                            size_t index = 0) {
    // static_assert(index < sizeof...(Ts) + 1, "Index out of range in parse_input");

    const auto placeholder = input.placeholder_positions[index].second;
    const auto fmt = values[index];

    auto res = parse_value_with_format<T>(fmt, placeholder);

    if (!res) {
        return std::unexpected(res.error());
    } else {
    }

    T value = *res;

    if constexpr (sizeof...(Ts) == 0) {
        return std::make_tuple(value);
    } else {
        auto tail = parse_input<Ts...>(input, values, index + 1);
        if (!tail) {
            return std::unexpected(tail.error());
        }
        return std::tuple_cat(std::make_tuple(value), *tail);
    }
}

}  // namespace stdx::details
