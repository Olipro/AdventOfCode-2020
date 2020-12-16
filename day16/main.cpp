#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class ValidityRange {
    int16_t min;
    int16_t max;
public:
    ValidityRange(int16_t min, int16_t max) : min{min}, max{max} {}

    [[nodiscard]] bool IsValid(int16_t value) const noexcept {
        return value >= min && value <= max;
    }
};

class Attribute {
    std::string name;
    ValidityRange first;
    ValidityRange second;
public:
    Attribute(std::string name, ValidityRange first, ValidityRange second) : name{std::move(name)}, first{first}, second{second} {}

    [[nodiscard]] bool IsValid(int16_t value) const noexcept {
        return first.IsValid(value) || second.IsValid(value);
    }

    [[nodiscard]] const std::string& Name() const noexcept {
        return name;
    }
};

class TicketAttributes {
    std::vector<Attribute> attributes;
public:
    explicit TicketAttributes(std::istream& in) {
        std::string name;
        int16_t min1, max1, min2, max2;
        while (in.peek() != '\n') {
            std::getline(in, name, ':');
            in >> min1;
            in.ignore(1);
            in >> max1;
            in.ignore(4);
            in >> min2;
            in.ignore(1);
            in >> max2;
            in.ignore();
            attributes.emplace_back(std::move(name), ValidityRange{min1, max1}, ValidityRange{min2, max2});
        }
        in.ignore();
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Puts us at our ticket.
    }

    [[nodiscard]] const auto& Attributes() const noexcept {
        return attributes;
    }
};

class Ticket {
    std::vector<uint16_t> values;
public:
    explicit Ticket(std::istream& in) {
        uint16_t value;
        while (in >> value) {
            values.emplace_back(value);
            if (in.get() == '\n')
                break;
        }
    }

    [[nodiscard]] const std::vector<uint16_t>& Values() const noexcept {
        return values;
    }
};

class Tickets {
    Ticket myTicket;
    std::vector<Ticket> otherTickets;
public:
    explicit Tickets(std::istream& in) : myTicket{in} {
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        while (in)
            otherTickets.emplace_back(in);
    }

    [[nodiscard]] const Ticket& MyTicket() const noexcept {
        return myTicket;
    }

    [[nodiscard]] const std::vector<Ticket>& OtherTickets() const noexcept {
        return otherTickets;
    }

    void DiscardInvalid(const TicketAttributes& attribs) {
        otherTickets.erase(std::remove_if(otherTickets.begin(), otherTickets.end(), [&attribs] (auto& ticket) {
            return std::ranges::any_of(ticket.Values(), [&attribs] (auto& value) {
                return std::ranges::none_of(attribs.Attributes(), [value] (auto& attrib) { return attrib.IsValid(value); });
            });
        }), otherTickets.end());
    }

    [[nodiscard]] int8_t ValidIndexForAttribute(const Attribute& attrib, const auto& stillUnknown) const noexcept {
        int8_t ret = -1;
        for (auto i = 0; i < myTicket.Values().size(); ++i) {
            if (stillUnknown.contains(i) && std::ranges::all_of(otherTickets, [i, &attrib] (auto& ticket) { return attrib.IsValid(ticket.Values()[i]); })) {
                if (ret == -1)
                    ret = i;
                else
                    return -1;
            }
        }
        return ret;
    }
};

class TicketMaster {
    TicketAttributes attributes;
    Tickets tickets;
public:
    explicit TicketMaster(std::istream& in) : attributes{in}, tickets{in} {}

    [[nodiscard]] uint32_t SolvePart1() const noexcept {
        auto& target = tickets.OtherTickets();
        return std::reduce(target.begin(), target.end(), 0, [this] (auto sum, auto& ticket) {
            auto& values = ticket.Values();
            return sum + std::reduce(values.begin(), values.end(), 0, [this] (auto sum, auto& value) {
               return sum + (std::ranges::none_of(attributes.Attributes(), [&value] (auto& attrib) { return attrib.IsValid(value); }) ? value : 0);
            });
        });
    }

    [[nodiscard]] uint64_t SolvePart2() noexcept {
        tickets.DiscardInvalid(attributes);
        std::unordered_map<int8_t, const Attribute*> unknownIndexes, knownIndexes;
        for (auto i = 0; i < tickets.MyTicket().Values().size(); ++i)
            unknownIndexes.emplace(i, &attributes.Attributes()[i]);
        while (!unknownIndexes.empty()) {
            for (auto& attr : unknownIndexes) {
                auto idx = tickets.ValidIndexForAttribute(*attr.second, unknownIndexes);
                if (idx != -1) {
                    knownIndexes[idx] = attr.second;
                    unknownIndexes[attr.first] = unknownIndexes[idx];
                    unknownIndexes.erase(idx);
                    break;
                }
            }
        }
        auto& values = tickets.MyTicket().Values();
        return std::reduce(knownIndexes.begin(), knownIndexes.end(), 1ull, [&values] (auto sum, auto& attrib) {
            return sum * (attrib.second->Name().find("departure") == 0 ? values[attrib.first] : 1);
        });
    }
};

int main(int argc, const char* argv[]) {
    if (argc != 2)
        return 1;
    std::ifstream file{argv[1]};
    TicketMaster tm{file};
    std::cout << tm.SolvePart1() << '\n';
    std::cout << tm.SolvePart2() << '\n';
    return 0;
}