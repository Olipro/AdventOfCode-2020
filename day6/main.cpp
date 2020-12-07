#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Group {
    std::unordered_map<char, uint8_t> answered;
    uint8_t total = 0;
public:
    explicit Group(std::istream& in) {
        std::string line;
        while (std::getline(in, line) && !line.empty()) {
            ++total;
            std::unordered_set<char> resps;
            for (auto c : line)
                resps.emplace(c);
            for (auto& c : resps)
                ++answered[c];
        }
    }

    friend unsigned operator+(unsigned a, const Group& b) {
        return a + b.answered.size();
    }

    friend unsigned long operator+(unsigned long a, const Group& b) {
        return a + std::ranges::count_if(b.answered, [t = b.total] (auto& kvp) { return kvp.second == t; });
    }
};

int main(int argc, const char* argv[]) {
    if (argc != 2)
        return 1;
    std::vector<Group> groups;
    std::ifstream file{argv[1]};
    while (file)
        groups.emplace_back(file);
    // Overly clever overload matching. Don't do this outside of toy code.
    // Fails to compile with libstdc++ until https://gcc.gnu.org/bugzilla/show_bug.cgi?id=95833 is fixed.
    std::cout << std::reduce(groups.begin(), groups.end(), 0u) << '\n';
    std::cout << std::reduce(groups.begin(), groups.end(), 0ul) << '\n';
}