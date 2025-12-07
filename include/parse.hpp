#pragma once

#include <charconv>
#include <expected>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "format_string.hpp"
#include "types.hpp"

namespace stdx::details {

template <typename T>
std::expected<void, details::scan_error> parse_value(std::string_view input, std::string_view fmt, T &value);

template <typename T>
requires std::is_integral_v<T>  // C++20
    std::expected<void, details::scan_error> parse_value(std::string_view input, std::string_view fmt, T &value) {
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
        return std::unexpected(details::scan_error{"Expected %f"});
    auto res = std::from_chars(input.data(), input.data() + input.size(), value);
    if (res.ec != std::errc{})
        return std::unexpected(details::scan_error{"Parse failed"});
    return {};
}

template <>
std::expected<void, details::scan_error> parse_value<double>(std::string_view input, std::string_view fmt,
                                                             double &value) {
    if (fmt[1] != 'f')
        return std::unexpected(details::scan_error{"Expected %f"});
    auto res = std::from_chars(input.data(), input.data() + input.size(), value);
    if (res.ec != std::errc{})
        return std::unexpected(details::scan_error{"Parse failed"});
    return {};
}

template <>
std::expected<void, details::scan_error> parse_value<std::string_view>(std::string_view input, std::string_view fmt,
                                                                       std::string_view &value) {
    if (fmt[1] != 's')
        return std::unexpected(details::scan_error{"Expected %s"});
    value = input;
    return {};
}

template <>
std::expected<void, details::scan_error> parse_value<std::string>(std::string_view input, std::string_view fmt,
                                                                  std::string &value) {
    if (fmt[1] != 's')
        return std::unexpected(details::scan_error{"Expected %s"});
    value = std::string(input);
    return {};
}

// Функция для парсинга значения с учетом спецификатора формата
template <typename T>
std::expected<T, scan_error> parse_value_with_format(std::string_view input, std::string_view fmt) {
    if (fmt.empty() || fmt.front() != '%' || fmt.size() < 2) {
        return std::unexpected(details::scan_error{"Wrong type"});
    }

    T value;
    auto result = parse_value(input, fmt, value);
    if (!result) {
        return std::unexpected(result);
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

templat<typename First, typename... Tail> auto
parse_input(std::vector<std::pair<size_t, std::string_view>> input_format)
    -> std::tuple<decltype(first_out), decltype(rest_out)...> {
    if (input_pairs.empty())
        return std::unexpected(details::scan_error{"No placeholders found"});

    auto pair = input_pairs.front();

    auto first_out = parse_single<First>(first_pair.first);  // парсим значение

    if constexpr (sizeof...(Rest) > 0) {
        return std::tuple_cat(std::make_tuple(first_out), parse_input(input_pairs, rest_out...));
    }
    return std::make_tuple(first_out);
}
}  // namespace stdx::details