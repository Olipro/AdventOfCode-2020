#include <fstream>
#include <iostream>
#include <set>
#include <vector>

class Adapter {
    uint16_t rating;
    uint64_t parentPaths;
public:
    explicit Adapter(uint16_t rating, uint64_t parentPaths) : rating{rating}, parentPaths{parentPaths} {}

    bool operator<(const Adapter& rhs) const noexcept {
        return rating < rhs.rating;
    }

    [[nodiscard]] int8_t Difference(const Adapter& rhs) const noexcept {
        return rhs.rating - rating;
    }

    [[nodiscard]] bool InRange(uint16_t val) const noexcept {
        return std::abs(rating - val) < 4;
    }

    [[nodiscard]] uint64_t ParentPaths() const noexcept {
        return parentPaths;
    }

    operator uint16_t() const noexcept {
        return rating;
    }
};

class Adapters {
    std::vector<Adapter> adapters;
public:
    explicit Adapters(std::istream& in) {
        int16_t rating;
        std::set<int16_t> data;
        adapters.reserve(150);
        while (in >> rating)
            data.emplace(rating);
        adapters.emplace_back(0, 1); // Wall Socket
        for (auto rating : data) {
            uint64_t reachableFrom = 0;
            auto i = 3;
            for (auto parent = adapters.rbegin(); parent != adapters.rend() && i > 0; ++parent, --i) {
                if (parent->InRange(rating))
                    reachableFrom += parent->ParentPaths();
            }
            adapters.emplace_back(rating, reachableFrom);
        }
        adapters.emplace_back(*adapters.rbegin() + 3, adapters.rbegin()->ParentPaths()); //Device
    }

    [[nodiscard]] int32_t CalculatePart1() const noexcept {
        auto oneDiff = 0, threeDiff = 0;
        for (auto next = adapters.begin(), i = next++; next != adapters.end(); ++i, ++next) {
            auto diff = i->Difference(*next);
            if (diff == 1)
                ++oneDiff;
            else if (diff == 3)
                ++threeDiff;
        }
        return oneDiff * threeDiff;
    }

    [[nodiscard]] uint64_t CalculatePart2() const noexcept {
        return adapters.rbegin()->ParentPaths();
    }
};

int main(int argc, const char* argv[]) {
    if (argc != 2)
        return 1;
    std::ifstream file{argv[1]};
    Adapters adapters{file};
    std::cout << adapters.CalculatePart1() << '\n';
    std::cout << adapters.CalculatePart2() << '\n';
}
