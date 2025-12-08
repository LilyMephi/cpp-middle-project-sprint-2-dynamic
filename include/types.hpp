#pragma once
#include <array>
#include <cstddef>
#include <stdexcept>
namespace stdx::details {

template <size_t N = 1024>
class fixed_string {
protected:
    std::array<char, N> data_;

public:
    constexpr fixed_string(const char *str, size_t len) {
        if (len > N)
            throw "Wirng size";
        for (size_t i = 0; i < len; ++i) {
            data_[i] = str[i];
        }
        if (len < N) {
            data_[len] = '\0';
        }
    }

    template <size_t M>
    requires (M <= N)
    constexpr fixed_string(const char (&str)[M]) noexcept {
        for (size_t i = 0; i < M; ++i) {
            data_[i] = str[i];
        }
        if (M < N) {
            data_[M] = '\0';
        }
    }

    constexpr size_t size() const noexcept { return N; }

    constexpr const char *data() const noexcept { return data_.data(); }

    constexpr std::string_view operator[](size_t idx) const noexcept {
        size_t word_count = 0;
        size_t start = 0;
        size_t end = 0;
        bool in_word = false;

        for (size_t i = 0; i < N; ++i) {
            if (data_[i] != ' ' && data_[i] != '\0') {
                if (!in_word) {
                    in_word = true;
                    start = i;
                }
            } else {
                if (in_word) {
                    end = i;
                    if (word_count == idx) {
                        return std::string_view(data_.data() + start, end - start);
                    }
                    ++word_count;
                    in_word = false;
                }
                if (data_[i] == '\0') {
                    break;
                }
            }
        }
        if (in_word) {
            end = N;
            if (word_count == idx) {
                return std::string_view(data_.data() + start, end - start);
            }
            ++word_count;
        }

        throw std::out_of_range("Word index out of range");
    }
};

struct scan_error {
    std::string message;
};

// class parse_error : public fixed_string<64> {
// public:
//     parse_error() = default;
//     explicit parse_error(const char *msg) : fixed_string<64>(msg) {}
// };

template <typename... Ts>
struct scan_result {
    std::tuple<Ts...> values;
    scan_result(std::tuple<Ts...> vals) : values(std::move(vals)) {}
};

}  // namespace stdx::details
