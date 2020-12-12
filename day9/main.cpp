#include <algorithm>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

template <size_t N>
constexpr int32_t permutations = (N * (N - 1)) / 2;

template <size_t N>
class NumberSum {
    int64_t number;
    std::vector<int64_t> sums;
public:
    explicit NumberSum(int64_t number) : number{number} {
        sums.reserve(permutations<N>);
    }

    int64_t SumAndPush(int64_t val) {
        return sums.emplace_back(number + val);
    }

    [[nodiscard]] const std::vector<int64_t> Sums() const noexcept {
        return sums;
    }

    operator int64_t() const noexcept {
        return number;
    }
};

template <size_t N>
class XMASCipher {
    std::vector<NumberSum<N>> data;
    std::unordered_multiset<int64_t> sumsSet;

    void ShiftStream(int32_t next) {
        for (auto& i : data[next - N].Sums())
            sumsSet.erase(sumsSet.find(i));
        auto& nextRef = data[next];
        for (auto i = data.begin() + (next - (N - 1)); i != data.begin() + next; ++i) {
            sumsSet.emplace(i->SumAndPush(nextRef));
        }
    }
public:
    explicit XMASCipher(std::istream& in) {
        int val;
        while (in >> val)
            data.emplace_back(val);
        sumsSet.reserve(permutations<N>);
        for (auto i = data.begin(); i != data.begin() + N; ++i) {
            for (auto j = i + 1; j != data.begin() + N; ++j) {
                sumsSet.emplace(i->SumAndPush(*j));
            }
        }
    }

    [[nodiscard]] int64_t FindFirstNonSumming() noexcept {
        for (auto i = N; i < data.size(); ++i) {
            if (!sumsSet.contains(data[i]))
                return data[i];
            ShiftStream(i);
        }
        return 0; //should never be reached.
    }

    [[nodiscard]] int64_t FindSumOfMinMaxMatching(const int64_t val) const noexcept {
        for (auto first = 0; first < data.size(); ++first) {
            int64_t sum = data[first], min = sum, max = sum;
            for (auto last = first + 1; last < data.size(); ++last) {
                int64_t elem = data[last];
                sum += elem;
                min = std::min(min, elem);
                max = std::max(max, elem);
                if (sum == val)
                    return min + max;
                else if (sum > val)
                    break;
            }
        }
        return 0; // should never be reached.
    }
};

int main(int argc, const char* argv[]) {
    if (argc != 2)
        return 1;
    std::ifstream file{argv[1]};
    XMASCipher<25> cipher{file};
    auto part1 = cipher.FindFirstNonSumming();
    auto part2 = cipher.FindSumOfMinMaxMatching(part1);
    std::cout << part1 << '\n' << part2 << '\n';
}