#pragma once
#include <array>
#include <cstddef>

namespace stdx::details {

template <size_t N = 1024>
class fixed_string {
protected:
    std::array<char, N> data_;

public:
    constexpr fixed_string(const char *str, size_t len) {
        if(len > N) throw "Wirng size";
        for (size_t i = 0; i < len; ++i) {
            data_[i] = str[i];
        }
        if (len < N) {
            data_[len] = '\0';
        }
    }

    // template <size_t M>
    // constexpr fixed_string(const char (&str)[M]) noexcept requires(M <= N) {
    //     for (size_t i = 0; i < N; ++i) {
    //         data_[i] = str[i];
    //     }
    // }

    constexpr size_t size() const noexcept { return N; }

    constexpr const char *data() const noexcept { return data_.data(); }

    constexpr char operator[](size_t i) const noexcept { return data_[i]; }
};

struct scan_error {
    std::string message;
};

// class parse_error : public fixed_string<64> {
// public:
//     parse_error() = default;
//     explicit parse_error(const char *msg) : fixed_string<64>(msg) {}
// };
// Шаблонный класс для хранения результатов успешного сканирования

template <typename... Ts>
struct scan_result {
    std::tuple<Ts...> values;
    // здесь ваш код

    class parse_error : public fixed_string<64> {
    public:
        parse_error() = default;
        explicit parse_error(const char *msg) : fixed_string<64>(msg) {}
    };
};

}  // namespace stdx::details
