#include <compare>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <set>
#include <string>

struct Point {
    int x;
    int y;
    constexpr Point(int x, int y) : x{x}, y{y} {}

    [[nodiscard]] bool operator<(const Point& rhs) const noexcept {
        return y < rhs.y || (y == rhs.y && x < rhs.x);
    }
};

class SparseMap {
    std::set<Point> trees;
    int xLen;
    int yLen;
public:
    explicit SparseMap(std::istream& in) {
        std::string line;
        for (auto y = 0; std::getline(in, line); ++y) {
            for (auto x = 0; x < line.size(); ++x) {
                if (line[x] == '#') {
                    trees.emplace(x, y);
                }
            }
        }
        xLen = line.size();
        yLen = trees.rbegin()->y + 1;
    }

    [[nodiscard]] constexpr int YLen() const noexcept {
        return yLen;
    }

    [[nodiscard]] constexpr bool IsOccupied(Point p) const noexcept {
        p.x %= xLen;
        return trees.contains(p);
    }

    [[nodiscard]] constexpr uint64_t CountCollisions(int xInc, int yInc) const noexcept {
        auto count = 0;
        for (auto y = yInc, x = xInc; y < YLen(); y += yInc, x += xInc) {
            if (IsOccupied({x, y}))
                ++count;
        }
        return count;
    }
};

int main(int argc, const char* argv[]) {
    if (argc != 2)
        return 1;
    std::ifstream file{argv[1]};
    const SparseMap map{file};
    auto part1 = map.CountCollisions(3, 1);
    std::cout << part1 << '\n';
    auto part2 = part1 *
                 map.CountCollisions(1, 1) *
                 map.CountCollisions(5, 1) *
                 map.CountCollisions(7, 1) *
                 map.CountCollisions(1, 2);
    std::cout << part2 << '\n';
}