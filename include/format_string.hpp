#pragma once
#include <array>
#include <expected>
#include <string_view>
#include <utility>
#include <vector>

#include "types.hpp"

namespace stdx {

template <size_t N = 1024>
class format_string : public details::fixed_string<N> {
public:
    static constexpr std::string_view placeholders[] = {"{%d}", "{%f}", "{%s}", "{%u}"};
    
    format_string(std::string_view str) : details::fixed_string<N>(str.data(), str.size()) {
        auto count_result = get_number_placeholders(str);
        if (count_result) {
            number_placeholders = *count_result;
        } else {
            number_placeholders = 0;
        }

        auto pos_result = get_placeholder_positions(str);
        if (pos_result) {
            placeholder_positions = std::move(*pos_result);
        }
    }
    static std::expected<size_t, details::scan_error> get_number_placeholders(std::string_view str) {
        if (str.empty()) {
            return std::unexpected(details::scan_error{"Wrong size "});
        }

        size_t count = 0;
        for (auto ph : placeholders) {
            size_t pos = 0;
            while ((pos = str.find(ph, pos)) != std::string_view::npos) {
                ++count;
                pos += ph.size();
            }
        }

        if (count == 0) {
            return std::unexpected(details::scan_error{"No placeholders found"});
        }

        return count;
    }

    static std::expected<std::vector<std::pair<size_t, std::string_view>>, details::scan_error>
    get_placeholder_positions(std::string_view str) {
        if (str.empty()) {
            return std::unexpected(details::scan_error{"Wrong size "});
        }

        std::vector<std::pair<size_t, std::string_view>> positions;

        size_t pos = 0;
        while (pos < str.size()) {
            bool found = false;
            for (auto ph : placeholders) {
                if (auto p = str.find(ph, pos); p != std::string_view::npos) {
                    positions.emplace_back(p, ph);
                    pos = p + ph.size();
                    found = true;
                    break;
                }
            }
            if (!found)
                break;
        }

        if (positions.empty()) {
            return std::unexpected(details::scan_error{"No placeholders found"});
        }
        return positions;
    }

    size_t number_placeholders;
    std::vector<std::pair<size_t, std::string_view>> placeholder_positions;
};


}  // namespace stdx

inline stdx::format_string<> operator"" _fs(const char* str, size_t len) {
  return stdx::format_string<>(std::string_view{str, len});
}