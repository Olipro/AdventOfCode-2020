#include <bitset>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>

#ifdef __has_builtin
#   if __has_builtin(__builtin_clzll)
#       define HAVE_CLZLL true
#   endif
#endif
#ifdef HAVE_CLZLL
    constexpr auto clzll = [] (auto val) { return __builtin_clzll(val); };
#else
#   define HAVE_CLZLL false
    constexpr auto clzll = [] (auto val) {};
#endif
constexpr bool haveClzll = HAVE_CLZLL;

template <size_t N, bool IsPart2>
class SizedMemory {
    std::unordered_map<uint64_t, std::bitset<N>> memory;
    std::bitset<N> andMask;
    std::bitset<N> orMask;
    std::bitset<N> floatMask;

    class MemoryElement {
        SizedMemory& mem;
        std::bitset<N>& element;
    public:
        MemoryElement(SizedMemory& mem, std::bitset<N>& element) noexcept : mem{mem}, element{element} {
            static_assert(N <= sizeof(uint64_t) * 8, "Sizes > 64 bit are not supported");
        }

        MemoryElement& operator=(uint64_t value) noexcept {
            if constexpr (!IsPart2)
                element = (std::bitset<N>{value} & mem.andMask) | mem.orMask;
            else
                element = std::bitset<N>{value};
            return *this;
        }
    };

    class MemoryDecoder {
        SizedMemory& mem;
        std::bitset<N> addr;

        [[nodiscard]] static uint8_t HighestBit(const std::bitset<N>& value) noexcept {
            if (value.none())
                return 0;
            if constexpr (haveClzll)
                return N - (clzll(value.to_ullong()) - ((sizeof(unsigned long long) * 8) - N));
            uint8_t pos = N;
            while (!value[--pos]);
            return pos + 1;
        }

        [[nodiscard]] std::bitset<N> CalcFloatPermutation(std::bitset<N> permute) const noexcept {
            std::bitset<N> ret;
            auto offset = 0;
            for (auto i = 0; i < HighestBit(permute); ++i) {
                while (!mem.floatMask[offset])
                    ++offset;
                ret[offset++] = permute[i];
            }
            return ret;
        }
    public:
        MemoryDecoder(SizedMemory& mem, uint64_t addr) noexcept : mem{mem}, addr{addr} {}
        MemoryDecoder& operator=(uint64_t value) noexcept {
            const auto baseAddr = (addr | mem.orMask) & ~mem.floatMask;
            for (auto i = 0; i < (1ull << mem.floatMask.count()); ++i)
                mem.memory[(baseAddr | CalcFloatPermutation(i)).to_ullong()] = value;
            return *this;
        }
    };
public:
    SizedMemory() {
        memory.reserve(70000);
    }

    [[nodiscard]] auto operator[](uint64_t addr) {
        if constexpr (!IsPart2)
            return MemoryElement{*this, memory[addr]};
        else
            return MemoryDecoder{*this, addr};
    }

    void SetMask(std::string_view bits) noexcept {
        andMask = 0xFFFFFFFFFFFFFFFF;
        orMask = 0;
        floatMask = 0;
        for (auto i = 0; i < bits.size(); ++i) {
            auto bit = *(bits.rbegin() + i);
            if (bit == '0')
                andMask[i] = false;
            else if (bit == '1')
                orMask[i] = true;
            else if (bit == 'X')
                floatMask[i] = true;
        }
    }

    [[nodiscard]] auto begin() const noexcept {
        return memory.begin();
    }

    [[nodiscard]] auto end() const noexcept {
        return memory.end();
    }
};

template <size_t N, bool IsPart2>
class ProgramExecutor {
    SizedMemory<N, IsPart2> memory;

    void HandleMask(std::istream& in) {
        std::string mask;
        std::getline(in, mask);
        memory.SetMask(mask);
    }

    void HandleWrite(std::istream& in) {
        uint64_t addr, value;
        in >> addr;
        in.ignore(4);
        in >> value;
        in.ignore(1);
        memory[addr] = value;
    }

    void Execute(std::istream& in) {
        char c[4];
        while (in) {
            in.read(c, 4);
            std::string_view cmd{c, 4};
            if (cmd == "mask")
                HandleMask(in);
            else if(cmd == "mem[")
                HandleWrite(in);
        }
    }
public:

    [[nodiscard]] uint64_t Solve(std::istream& in) {
        Execute(in);
        constexpr auto reducer = [] (auto a, auto& b) { return a + b.second.to_ullong(); };
        return std::reduce(memory.begin(), memory.end(), 0ull, reducer);
    }
};

int main(int argc, const char* argv[]) {
    if (argc != 2)
        return 1;
    std::ifstream file{argv[1]};
    {
        ProgramExecutor<36, false> part1;
        std::cout << part1.Solve(file) << '\n';
    }

    file.clear();
    file.seekg(0);

    {
        ProgramExecutor<36, true> part2;
        std::cout << part2.Solve(file) << '\n';
    }
}
