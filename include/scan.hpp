#pragma once

#include "parse.hpp"
#include "types.hpp"
// #include <string_view>

namespace stdx {


template <typename... Ts>
std::expected<std::tuple<Ts...>, details::scan_error> scan(format_string<> input, details::fixed_string<> format) {
    if constexpr (sizeof...(Ts) > 0) {
        return details::parse_input<Ts...>(input, format);
    } else {
        return std::unexpected(details::scan_error{"No types specified"});
    }
}

}  // namespace stdx
