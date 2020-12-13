#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <utility>

enum class PositionState {
    INVALID,
    FLOOR,
    AVAILABLE,
    OCCUPIED,
};

class Coord {
    int16_t x;
    int16_t y;
public:
    Coord(int16_t x, int16_t y) noexcept : x{x}, y{y} {}

    [[nodiscard]] int16_t X() const noexcept {
        return x;
    }

    [[nodiscard]] int16_t Y() const noexcept {
        return y;
    }

    bool operator==(const Coord& rhs) const noexcept {
        return x == rhs.x && y == rhs.y;
    }

    struct Hash {
        auto operator()(const Coord& coord) const noexcept {
            return std::hash<int32_t>{}((static_cast<int32_t>(coord.x) << 16) | coord.y);
        }
    };
};

class SpaceArrangement {
    std::unordered_map<Coord, PositionState, Coord::Hash> positions;
    int16_t xMax;
    int16_t yMax;

    template <class Predicate>
    [[nodiscard]] uint8_t IsInState(int16_t x, int16_t y, Predicate& pred) const noexcept {
        auto pos = positions.find(Coord{x, y});
        return pred(pos == positions.end() ? PositionState::INVALID : pos->second) ? 1 : 0;
    }

    [[nodiscard]] auto RaycastToChair(int16_t x, int16_t y, int16_t xDiff, int16_t yDiff, int16_t xLimit, int16_t yLimit) const noexcept {
        while (x != xLimit && y != yLimit) {
            x += xDiff;
            y += yDiff;
            if (auto pos = positions.find(Coord{x, y}); pos != positions.end())
                return pos;
        }
        return positions.end();
    }

    template <class Predicate>
    [[nodiscard]] bool CountRaycastsInState(int16_t x, int16_t y, int8_t target, Predicate&& pred) const noexcept {
        auto count = 0;
        std::initializer_list<std::tuple<int16_t, int16_t, int16_t, int16_t>> consider = {{0, -1, -1, 0}, // up
                                                                                          {0, 1, -1, yMax}, // down
                                                                                          {-1, 0, 0, -1}, // left
                                                                                          {1, 0, xMax, -1}, // right
                                                                                          {-1, -1, 0, 0}, // up&left
                                                                                          {1, -1, xMax, 0}, // up&right
                                                                                          {-1, 1, 0, yMax}, // down&left
                                                                                          {1, 1, xMax, yMax}, // down&right
                                                                                          };
        for (auto& [xdt, ydt, xLim, yLim] : consider) {
            auto pos = RaycastToChair(x, y, xdt, ydt, xLim, yLim);
            count += pred(pos == positions.end() ? PositionState::INVALID : pos->second) ? 1 : 0;
            if (count == target)
                return true;
        }
        return false;
    }

    template <class Predicate>
    [[nodiscard]] bool CountNeighboursInState(int16_t x, int16_t y, int8_t target, Predicate&& pred) const noexcept {
        std::initializer_list<std::pair<int16_t, int16_t>> consider = {{x, y - 1},
                                                                       {x, y + 1},
                                                                       {x - 1, y},
                                                                       {x + 1, y},
                                                                       {x - 1, y - 1},
                                                                       {x + 1, y - 1},
                                                                       {x - 1, y + 1},
                                                                       {x + 1, y + 1}};
        auto count = 0;
        for (auto& [x, y] : consider) {
            count += IsInState(x, y, pred);
            if (count == target)
                return true;
        }
        return false;
    }
    
    [[nodiscard]] bool AllUnoccupied(int16_t x, int16_t y, bool raycast) const noexcept {
        constexpr auto pred = [] (auto state) { return state != PositionState::OCCUPIED; };
        return raycast ? CountRaycastsInState(x, y, 8, pred) :
                         CountNeighboursInState(x, y, 8, pred);
    }

    [[nodiscard]] bool ShouldBecomeEmpty(const auto& kvp, bool raycast, int8_t target) const noexcept {
        auto& [coord, state] = kvp;
        constexpr auto pred = [] (auto state) { return state == PositionState::OCCUPIED; };
        return state == PositionState::OCCUPIED &&
            (raycast ? CountRaycastsInState(coord.X(), coord.Y(), target, pred) :
                       CountNeighboursInState(coord.X(), coord.Y(), target, pred));
    }
    
    [[nodiscard]] bool ShouldBecomeOccupied(const auto& kvp, bool raycast) const noexcept {
        auto& [coord, state] = kvp;
        return state == PositionState::AVAILABLE && AllUnoccupied(coord.X(), coord.Y(), raycast);
    }
    
    [[nodiscard]] std::pair<decltype(positions), bool> Simulate(bool raycast) {
        auto newPositions = positions;
        auto changed = false;
        for (auto& kvp : positions) {
            auto& [coord, state] = kvp;
            if (ShouldBecomeOccupied(kvp, raycast)) {
                newPositions[coord] = PositionState::OCCUPIED;
                changed = true;
            }
            else if (ShouldBecomeEmpty(kvp, raycast, raycast ? 5 : 4)) {
                newPositions[coord] = PositionState::AVAILABLE;
                changed = true;
            }
        }
        return {std::move(newPositions), changed};
    }
public:
    explicit SpaceArrangement(std::istream& in) {
        char c;
        int16_t x = 0, y = 0;
        while (c = in.get(), in) {
            if (c == 'L')
                positions.emplace(Coord{x++, y}, PositionState::AVAILABLE);
            else if (c == '\n')
                x = 0, ++y;
            else
                ++x;
        }
        yMax = y;
        xMax = x - 1;
    }

    [[nodiscard]] int16_t SolvePart(bool isPart1) {
        auto savedState = positions;
        while (true) {
            auto&& [newPos, changed] = Simulate(isPart1);
            if (!changed)
                break;
            positions = std::move(newPos);
        }
        auto ret = std::ranges::count_if(positions, [] (auto& pos) { return pos.second == PositionState::OCCUPIED; });
        positions = std::move(savedState);
        return ret;
    }
};

int main(int argc, const char* argv[]) {
    if (argc != 2)
        return 1;
    std::ifstream file{argv[1]};
    SpaceArrangement arrangement{file};
    std::cout << arrangement.SolvePart(false) << '\n';
    std::cout << arrangement.SolvePart(true) << '\n';
}