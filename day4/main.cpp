#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>

class Passport {
    using CharKeys = std::initializer_list<const char[4]>;
    static constexpr CharKeys requiredFields = {"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid"};
    static constexpr CharKeys validEyes = {"amb", "blu", "brn", "gry", "grn", "hzl", "oth"};

    std::unordered_map<std::string, std::string> attrs;

    [[nodiscard]] constexpr static size_t FindEnd(const std::string& line, int offset) {
        auto end = line.find(' ', offset);
        if (end != std::string::npos)
            return end - offset;
        return line.size() - offset;
    }

    void ParseLine(const std::string& line) {
        for (auto i = 0; i < line.size();) {
            auto key = line.substr(i, 3);
            auto valLen = FindEnd(line, i + 4);
            auto val = line.substr(i + 4, valLen);
            attrs[std::move(key)] = std::move(val);
            i += 4 + valLen + 1;
        }
    }

    template <typename Func>
    [[nodiscard]] bool ValidateKey(const char key[4], Func&& func) const noexcept {
        auto kvp = attrs.find(key);
        return kvp != attrs.end() && func(kvp->second);
    }

    [[nodiscard]] bool ValidateInt(const char key[4], const int min, const int max) const noexcept {
        return ValidateKey(key, [min, max] (auto& str) {
            auto val = std::atoi(str.c_str());
            return val >= min && val <= max;
        });
    }

    [[nodiscard]] bool ValidateHeight() const noexcept {
        return ValidateKey("hgt", [] (auto& hgt) {
            if (hgt.size() < 4)
                return false;
            auto unit = std::string_view{hgt}.substr(hgt.size() - 2);
            auto len = std::atoi(hgt.substr(0, hgt.size() - 2).c_str());
            return (unit == "cm" && len > 149 && len < 194) || (unit == "in" && len > 58 && len < 77);
        });
    }

    [[nodiscard]] bool ValidateHair() const noexcept {
        return ValidateKey("hcl", [this] (std::string_view hcl) {
            return hcl.size() == 7 && hcl[0] == '#' &&
                   std::ranges::all_of(hcl.substr(1), [](auto &i) { return i >= '0' && i <= 'f'; });
        });
    }

    [[nodiscard]] bool ValidateEyes() const noexcept {
        return ValidateKey("ecl", [] (auto& ecl) {
            return ecl.size() == 3 && std::ranges::any_of(validEyes, [&ecl](const auto &col) { return col == ecl; });
        });
    }

    [[nodiscard]] bool ValidatePID() const noexcept {
        return ValidateKey("pid", [] (auto& pid) {
            return pid.size() == 9 && std::ranges::all_of(pid, [](auto &dgt) { return dgt >= '0' && dgt <= '9'; });
        });
    }

public:
    explicit Passport(std::istream& in) {
        attrs.reserve(requiredFields.size());
        std::string line;
        while (std::getline(in, line) && !line.empty()) {
            ParseLine(line);
        }
    }

    [[nodiscard]] bool IsValid() const noexcept {
        return std::ranges::all_of(requiredFields, [this] (const auto& i) { return attrs.contains(i); });
    }

    [[nodiscard]] bool IsValid2() const noexcept {
        return ValidateInt("byr", 1920, 2002) && ValidateInt("iyr", 2010, 2020) && ValidateInt("eyr", 2020, 2030) &&
               ValidateHeight() && ValidateHair() && ValidateEyes() && ValidatePID();
    }
};

int main(int argc, const char* argv[]) {
    if (argc != 2)
        return 1;
    std::ifstream file{argv[1]};
    auto part1 = 0, part2 = 0;
    while (file) {
        Passport passport{file};
        if (passport.IsValid())
            ++part1;
        if (passport.IsValid2())
            ++part2;
    }
    std::cout << part1 << '\n' << part2 << '\n';
    return 0;
}