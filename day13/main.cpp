#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>

class Bus {
    int64_t id;
    int64_t offset;
    int64_t waitTime;

    [[nodiscard]] int64_t CalcWaitTime(int64_t timestamp) const noexcept {
        auto mod = timestamp % id;
        return mod == 0 ? 0 : id - mod;
    }
public:
    Bus(int64_t id, int64_t offset, int64_t timestamp) noexcept :
        id{id}, offset{offset}, waitTime{CalcWaitTime(timestamp)} {}

    [[nodiscard]] int64_t ID() const noexcept {
        return id;
    }

    [[nodiscard]] int64_t Offset() const noexcept {
        return offset;
    }

    [[nodiscard]] int64_t WaitTime() const noexcept {
        return waitTime;
    }

    [[nodiscard]] bool operator<(const Bus& rhs) const noexcept {
        return waitTime < rhs.waitTime;
    }
};

class Buses {
    std::vector<Bus> buses;
    int64_t timestamp;
    int64_t mult = 1;

    static int64_t FindModMult(int64_t val, int64_t id) {
        int i;
        for (i = 1; ((val * i) % id) != 1; ++i);
        return i;
    }
public:
    explicit Buses(std::istream& in) {
        in >> timestamp;
        int busId, i = 0;
        while (!in.eof()) {
            in >> busId;
            if (in) {
                in.ignore(1);
                mult *= busId;
                buses.emplace_back(busId, i++, timestamp);
            } else {
                in.clear();
                in.ignore(2);
                ++i;
            }
        }
    }

    [[nodiscard]] int64_t SolvePart1() const noexcept {
        auto bus = std::min_element(buses.begin(), buses.end());
        return bus->WaitTime() * bus->ID();
    }

    [[nodiscard]] int64_t SolvePart2() const noexcept {
        std::vector<int64_t> reciprocals;
        std::vector<int64_t> modOneMult;
        reciprocals.reserve(buses.size()), modOneMult.reserve(buses.size());
        for (auto& bus : buses)
            reciprocals.emplace_back(mult / bus.ID());
        for (auto i = 0; i < buses.size(); ++i)
            modOneMult.emplace_back(FindModMult(reciprocals[i], buses[i].ID()));
        int64_t result = 0;
        for (auto i = 0; i < buses.size(); ++i) {
            auto tgtMod = (buses[i].ID() - buses[i].Offset()) % buses[i].ID();
            result += (tgtMod * reciprocals[i] * modOneMult[i]);
        }
        return result % mult;
    }
};

int main(int argc, const char* argv[]) {
    if (argc != 2)
        return 1;
    std::ifstream file{argv[1]};
    Buses buses{file};
    std::cout << buses.SolvePart1() << '\n';
    std::cout << buses.SolvePart2() << '\n';
    return 0;
}