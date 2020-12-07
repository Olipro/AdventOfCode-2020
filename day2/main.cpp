#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

class PasswordPolicy {
    unsigned int min;
    unsigned int max;
    char chr;
public:
    PasswordPolicy(decltype(min) min, decltype(max) max, decltype(chr) chr) : min{min}, max{max}, chr{chr} {}

    [[nodiscard]] bool IsValid(std::string_view password) const noexcept {
        auto count = std::count_if(password.begin(), password.end(), [this] (auto& c) { return c == chr; });
        return count >= min && count <= max;
    }

    [[nodiscard]] bool IsValid2(std::string_view password) const noexcept {
        return password[min-1] == chr ^ password[max-1] == chr;
    }
};

int main(int argc, const char* argv[]) {
    if (argc != 2)
        return 1;
    std::ifstream file{argv[1]};
    auto totalValid = 0, totalValid2 = 0;
    while (file) {
        unsigned int min, max;
        char chr;
        file >> min;
        file.ignore(1);
        file >> max;
        file.ignore(1);
        file >> chr;
        file.ignore(2);
        std::string password;
        std::getline(file, password);
        PasswordPolicy policy{min, max, chr};
        if (policy.IsValid(password))
            ++totalValid;
        if (policy.IsValid2(password))
            ++totalValid2;
    }
    std::cout << totalValid << '\n' << totalValid2 << '\n';
}