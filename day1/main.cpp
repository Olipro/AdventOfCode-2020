#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

template <int depth>
std::optional<int> FindSum(const int target, auto begin, auto end, const int init = 0) {
    for (; begin != end; ++begin) {
        auto val = *begin;
        if constexpr (depth > 0) {
            if (init + val <= target) {
                if (auto val2 = FindSum<depth - 1>(target, begin + 1, end, init + val); val2) {
                    return *val2 * val;
                }
            }
        } else if (init + val == target) {
            return val;
        }
    }
    return std::nullopt;
}

int main(int argc, const char* argv[]) {
    if (argc != 2)
        return 1;
    std::string line;
    std::ifstream file{argv[1]};
    std::vector<int> queue;
    while (std::getline(file, line)) {
        queue.emplace_back(std::stoi(line));
    }
    std::cout << *FindSum<1>(2020, queue.cbegin(), queue.cend()) << '\n'
              << *FindSum<2>(2020, queue.cbegin(), queue.cend()) << '\n';
    return 0;
}
