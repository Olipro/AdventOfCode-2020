#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_set>

class Number {
    uint32_t value;
    mutable uint32_t turnLastSpoken = 0;
    mutable uint32_t turnPenultimateSpoken = 0;
    mutable uint32_t timesSpoken = 0;
public:
    explicit Number(uint32_t value) noexcept : value{value} {}

    [[nodiscard]] uint32_t Diff() const noexcept {
        return turnLastSpoken - turnPenultimateSpoken;
    }

    uint32_t TimesSpoken() const noexcept {
        return timesSpoken;
    }

    void TurnLastSpoken(uint32_t val) const noexcept {
        turnPenultimateSpoken = turnLastSpoken;
        ++timesSpoken;
        turnLastSpoken = val;
    }

    operator uint32_t() const noexcept {
        return value;
    }

    bool operator==(const Number& rhs) const noexcept {
        return value == rhs.value;
    }

    struct Hash {
        auto operator()(const Number& number) const noexcept {
            return std::hash<uint32_t>{}(number);
        }
    };
};

class MemoryGame {
    std::unordered_set<Number, Number::Hash> numbers;
    const Number* lastSpoken;
    uint32_t currentTurn = 0;
public:
    explicit MemoryGame(std::istream& in) {
        uint32_t value;
        while (in >> value) {
            auto elem = numbers.emplace(value);
            lastSpoken = &*elem.first;
            lastSpoken->TurnLastSpoken(++currentTurn);
            in.ignore(1);
        }
    }
    MemoryGame(const MemoryGame& rhs) :
        numbers{rhs.numbers}, lastSpoken{&*numbers.find(*rhs.lastSpoken)}, currentTurn{rhs.currentTurn} {
        numbers.reserve(4'000'000);
    }

    void DoTurn() {
        if (lastSpoken->TimesSpoken() == 1) {
            auto& elem = *numbers.emplace(0).first;
            elem.TurnLastSpoken(++currentTurn);
            lastSpoken = &elem;
        } else {
            auto& elem = *numbers.emplace(lastSpoken->Diff()).first;
            elem.TurnLastSpoken(++currentTurn);
            lastSpoken = &elem;
        }
    }

    uint32_t GetLastSpokenForTurn(uint32_t turn) {
        while (currentTurn != turn)
            DoTurn();
        return *lastSpoken;
    }
};

int main(int argc, const char* argv[]) {
    if (argc != 2)
        return 1;
    std::ifstream file{argv[1]};
    MemoryGame part1{file};
    MemoryGame part2 = part1;
    std::cout << part1.GetLastSpokenForTurn(2'020) << '\n';
    std::cout << part2.GetLastSpokenForTurn(30'000'000) << '\n';
}
