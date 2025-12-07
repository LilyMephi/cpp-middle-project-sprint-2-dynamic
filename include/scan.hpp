#pragma once

#include "parse.hpp"
#include "types.hpp"
// #include <string_view>

namespace stdx {


// замените болванку функции scan на рабочую версию
template <typename... Ts>
std::expected<details::scan_result<Ts...>, details::scan_error> scan(format_string<> input, details::fixed_string<> format) {
    size_t size_place_holder = input.number_placeholders;
    if (size_place_holder != format.size())
        return std::unexpected(details::scan_error{"Wrong size of plaace holders or types"});

    auto placeholder_positions = input.placeholder_positions;

    details::scan_result res{};
    for(auto hold_pos : placeholder_positions){
        res.values
    }
}

}  // namespace stdx
