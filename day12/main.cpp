#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>

class Movable {
    int16_t dir = 90;
    int32_t x = 0;
    int32_t y = 0;
    static const inline double radian = std::acos(-1) / 180.;
public:
    Movable(int32_t x, int32_t y) noexcept : x{x}, y{y} {}

    constexpr void Go(int32_t num, int32_t d) noexcept {
        if (d == 0)
            y -= num;
        else if (d == 180)
            y += num;
        else if (d == 90)
            x += num;
        else if (d == 270)
            x -= num;
    }

    constexpr void Turn(int32_t num) noexcept {
        dir = (dir + num) % 360;
        if (dir < 0)
            dir += 360;
    }

    void TurnRelativeTo(Movable& ship, int32_t degrees) {
        auto sin = static_cast<int>(std::sin(radian * degrees));
        auto cos = static_cast<int>(std::cos(radian * degrees));
        auto newX = x - ship.x;
        auto newY = y - ship.y;
        x = ((newX * cos) - (newY * sin)) + ship.x;
        y = ((newX * sin) + (newY * cos)) + ship.y;
    }

    constexpr void TravelTo(Movable& waypoint, int32_t times) noexcept {
        auto xDt = (waypoint.x - x) * times;
        auto yDt = (waypoint.y - y) * times;
        x += xDt;
        y += yDt;
        waypoint.x += xDt;
        waypoint.y += yDt;
    }

    [[nodiscard]] int32_t Dir() const noexcept {
        return dir;
    }

    [[nodiscard]] int32_t X() const noexcept {
        return x;
    }

    [[nodiscard]] int32_t Y() const noexcept {
        return y;
    }

    [[nodiscard]] int32_t DistanceFromOrigin() const noexcept {
        return std::abs(x) + std::abs(y);
    }
};

void SolvePart1(Movable& ship, std::istream& in) {
    char cmd;
    int16_t num;
    while (in >> cmd >> num) {
        if (cmd == 'F')
            ship.Go(num, ship.Dir());
        else if (cmd == 'N')
            ship.Go(num, 0);
        else if (cmd == 'S')
            ship.Go(num, 180);
        else if (cmd == 'E')
            ship.Go(num, 90);
        else if (cmd == 'W')
            ship.Go(num, 270);
        else if (cmd == 'R')
            ship.Turn(num);
        else if (cmd == 'L')
            ship.Turn(-num);
    }
}

void SolvePart2(Movable& ship, Movable& waypoint, std::istream& in) {
    char cmd;
    int16_t num;
    while (in >> cmd >> num) {
        if (cmd == 'F')
            ship.TravelTo(waypoint, num);
        else if (cmd == 'N')
            waypoint.Go(num, 0);
        else if (cmd == 'S')
            waypoint.Go(num, 180);
        else if (cmd == 'E')
            waypoint.Go(num, 90);
        else if (cmd == 'W')
            waypoint.Go(num, 270);
        else if (cmd == 'R')
            waypoint.TurnRelativeTo(ship, num);
        else if (cmd == 'L')
            waypoint.TurnRelativeTo(ship, -num);
    }
}

int main(int argc, const char* argv[]) {
    if (argc != 2)
        return 1;
    std::ifstream file{argv[1]};
    Movable ship{0, 0};
    SolvePart1(ship, file);
    std::cout << ship.DistanceFromOrigin() << '\n';
    file.clear();
    file.seekg(0);
    ship = Movable{0, 0};
    Movable waypoint{10, -1};
    SolvePart2(ship, waypoint, file);
    std::cout << ship.DistanceFromOrigin() << '\n';
}