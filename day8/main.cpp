#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

enum class OpCode {
    ACC,
    JMP,
    NOP
};

struct Op {
    OpCode code;
    int32_t arg1;
    Op(OpCode code, int32_t arg1) : code{code}, arg1{arg1} {}
};

class TuringMachine {
    std::vector<Op> insns; // "instructions"
    int32_t acc = 0; // Accumulator
    uint32_t ip = 0; // Instruction Pointer
public:
    explicit TuringMachine(std::vector<Op>&& insns) noexcept : insns{std::move(insns)} {}

    void ExecClockCycle() noexcept {
        auto& op = insns[ip++];
        switch (op.code) {
            case OpCode::ACC:
                acc += op.arg1;
                break;
            case OpCode::JMP:
                ip += op.arg1 - 1;
                break;
            case OpCode::NOP:
                break;
        }
    }

    void Reset() noexcept {
        ip = acc = 0;
    }

    void ChangeInsn(uint32_t idx, OpCode newCode) noexcept {
        insns[idx].code = newCode;
    }

    [[nodiscard]] const std::vector<Op>& Insns() const noexcept {
        return insns;
    }

    [[nodiscard]] uint32_t IP() const noexcept {
        return ip;
    }

    [[nodiscard]] int32_t Acc() const noexcept {
        return acc;
    }
};

class Simulator {
    TuringMachine tm;

    [[nodiscard]] static std::vector<Op> InitMachine(std::istream& in) {
        std::vector<Op> ret;
        ret.reserve(623);
        std::string op;
        int arg;
        while (in >> op >> arg) {
            if (op == "jmp")
                ret.emplace_back(OpCode::JMP, arg);
            else if (op == "acc")
                ret.emplace_back(OpCode::ACC, arg);
            else if (op == "nop")
                ret.emplace_back(OpCode::NOP, arg);
        }
        return ret;
    }

    void SwapJmpNop(uint32_t pos) noexcept {
        tm.ChangeInsn(pos, tm.Insns()[pos].code == OpCode::NOP ? OpCode::JMP : OpCode::NOP);
    }

    [[nodiscard]] uint32_t TryInsnSwap(uint32_t lastPos) noexcept {
        auto& insns = tm.Insns();
        if (lastPos > 0)
            SwapJmpNop(lastPos - 1);
        while (insns[lastPos++].code == OpCode::ACC);
        SwapJmpNop(lastPos - 1);
        return lastPos;
    }
public:
    explicit Simulator(std::istream& in) : tm{InitMachine(in)} {}

    [[nodiscard]] uint32_t GetAccOnFirstRepetition() noexcept {
        std::unordered_set<uint32_t> visited;
        while (tm.IP() < tm.Insns().size() && !visited.contains(tm.IP())) {
            visited.emplace(tm.IP());
            tm.ExecClockCycle();
        }
        return tm.Acc();
    }

    [[nodiscard]] uint32_t FindAndFix() noexcept {
        uint32_t lastChangePos = 0, acc;
        auto& insns = tm.Insns();
        while (tm.IP() != insns.size()) {
            lastChangePos = TryInsnSwap(lastChangePos);
            tm.Reset();
            acc = GetAccOnFirstRepetition();
        }
        return acc;
    }
};

int main(int argc, const char* argv[]) {
    if (argc != 2)
        return 1;
    std::ifstream file{argv[1]};
    Simulator sim{file};
    std::cout << sim.GetAccOnFirstRepetition() << '\n';
    std::cout << sim.FindAndFix() << '\n';
}