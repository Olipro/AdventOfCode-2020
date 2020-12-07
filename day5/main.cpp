#include <cstdint>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <utility>

class Seat {
    uint16_t id_;

    static constexpr uint16_t Diff(int lower, int upper) noexcept {
        return (upper - lower) >> 1;
    }

    [[nodiscard]] static constexpr int FindLocation(std::string_view str, const char lChr) noexcept {
        auto lower = 0;
        auto upper = (1 << str.size()) - 1;
        for (auto i = 0; i < str.size() - 1; ++i) {
            if (str[i] == lChr)
                upper = lower + Diff(lower, upper);
            else
                lower += Diff(lower, upper) + 1;
        }
        return str.back() == lChr ? lower : upper;
    }

    [[nodiscard]] static constexpr uint16_t GetID(std::string_view pos) noexcept {
        auto row = FindLocation(pos.substr(0, 7), 'F');
        auto col = FindLocation(pos.substr(7), 'L');
        return (row * 8) + col;
    }
public:
    explicit constexpr Seat(std::string_view pos) noexcept : id_{GetID(pos)} {}
    constexpr Seat(uint16_t id) noexcept : id_{id} {}

    [[nodiscard]] unsigned ID() const noexcept {
        return id_;
    }

    std::strong_ordering operator<=>(const Seat&) const noexcept = default;
};

[[nodiscard]] uint16_t FindSeat(const std::set<Seat>& seats) {
    for (auto pos = 9; pos < 127 * 8; ++pos) {
        if (!seats.contains(pos) && seats.contains(pos - 1) && seats.contains(pos + 1))
            return pos;
    }
    throw std::runtime_error{"No Seat found for part 2."};
}

int main(int argc, const char* argv[]) {
    if (argc != 2)
        return 0;
    std::set<Seat> seats;
    std::ifstream file{argv[1]};
    std::string line;
    while (std::getline(file, line))
        seats.emplace(line);
    std::cout << seats.rbegin()->ID() << '\n';
    std::cout << FindSeat(seats) << '\n';
}