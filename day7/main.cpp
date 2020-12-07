#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

class Bag {
    std::string name;
    mutable std::unordered_map<std::string, uint8_t> containedBags;
public:
    Bag(std::string name) : name{std::move(name)} {}

    void AddBag(const Bag& bag, uint8_t count) const {
        containedBags.emplace(bag.name, count);
    }

    const std::string& Name() const noexcept {
        return name;
    }

    const decltype(containedBags)& ContainedBags() const noexcept {
        return containedBags;
    }

    bool operator==(const Bag& rhs) const noexcept {
        return name == rhs.name;
    }

    bool operator!=(const std::string& name) const noexcept {
        return this->name != name;
    }

    struct Hash {
        size_t operator()(const Bag& bag) const noexcept {
            return std::hash<std::string>{}(bag.name);
        }
    };
};

class Bags {
    std::unordered_set<Bag, Bag::Hash> bags{594};

    const Bag& AddBag(std::istream& in) {
        std::string hue, colour;
        in >> hue >> colour;
        return *bags.emplace(std::move(hue.append(colour))).first;
    }

    void AddSubBags(std::istream& in, const Bag& parent) {
        int count;
        std::string end;
        while (true) {
            in >> count;
            if (!in) {
                in.clear();
                std::getline(in, end);
                return;
            }
            auto& child = AddBag(in);
            parent.AddBag(child, count);
            in >> end;
            if (end.back() == '.')
                break;
        }
    }

    [[nodiscard]] bool FindRecurse(const Bag& bag, const std::string& target) const noexcept {
        auto& children = bag.ContainedBags();
        return children.contains(target) || std::ranges::any_of(children,[this, &target] (auto& child) {
            return FindRecurse(*bags.find(child.first), target);
        });
    }

    [[nodiscard]] int CountRecurse(const Bag& bag, int mult = 1) const noexcept {
        auto& children = bag.ContainedBags();
        return std::reduce(children.begin(), children.end(), 0, [this, mult] (auto a, auto& child) {
            return a + (mult * child.second) + CountRecurse(*bags.find(child.first), mult * child.second);
        });
    }
public:
    explicit Bags(std::istream& in) {
        std::string discard;
        while (!in.eof()) {
            auto& bag = AddBag(in);
            in.seekg(14, std::ios_base::cur);
            AddSubBags(in, bag);
        }
    }

    [[nodiscard]] int CountCanHold(const std::string& name) const noexcept {
        std::unordered_set<std::string_view> validEntries{bags.size()};
        for (auto& bag : bags) {
            if (bag != name && FindRecurse(bag, name))
                validEntries.emplace(bag.Name());
        }
        return validEntries.size();
    }

    [[nodiscard]] int CountHolds(const std::string& name) const noexcept {
        return CountRecurse(*bags.find(name));
    }
};

int main(int argc, const char* argv[]) {
    if (argc != 2)
        return 1;
    std::ifstream file{argv[1]};
    Bags bags{file};
    std::cout << bags.CountCanHold("shinygold") << '\n';
    std::cout << bags.CountHolds("shinygold") << '\n';
    return 0;
}
