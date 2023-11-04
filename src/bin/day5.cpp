#include <format>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

struct Instruction {
    std::size_t count;
    char from;
    char to;
};

struct Expect {
    Expect(std::string_view expected) : expected{expected} {}
    std::string_view expected;
};
std::istream& operator>>(std::istream& is, Expect&& ch) {
    static thread_local std::string tmp;
    is >> tmp;
    if (tmp != ch.expected) throw std::runtime_error(std::format("Expected \"{}\" on the input stream", ch.expected));
    return is;
}

int main() {
    std::vector<std::string> state;
    for (std::string line; std::getline(std::cin, line);) {
        if (line.empty()) break;
        state.push_back(line);
    }

    std::vector<char> sequence;
    std::unordered_map<char, std::stack<char>> inputStacks;
    const auto& idLine = state.back();
    for (std::size_t i = 0; i < idLine.size(); ++i) {
        const auto ch = idLine[i];
        if (std::isspace(ch)) continue;

        sequence.push_back(ch);
        auto& stack = inputStacks[ch];
        for (int j = state.size() - 1; j >= 0; j--) {
            if (std::isspace(state[j][i])) break;

            stack.push(state[j][i]);
        }
    }

    std::vector<Instruction> instructions;
    for (std::string line; std::getline(std::cin, line);) {
        std::istringstream iss{line};
        Instruction i;
        iss >> Expect("move") >> i.count >> Expect("from") >> i.from >> Expect("to") >> i.to;
        instructions.push_back(std::move(i));
    }

    auto stacks = inputStacks;
    for (const auto& instruction : instructions) {
        auto& from = stacks[instruction.from];
        auto& to = stacks[instruction.to];
        for (std::size_t i = 0; i < instruction.count; ++i) {
            auto tmp = from.top();
            from.pop();
            to.push(tmp);
        }
    }

    const auto getResult = [](const auto& sequence, const auto& stacks) {
        std::string result(sequence.size(), '?');
        for (std::size_t i = 0; i < sequence.size(); ++i) {
            result[i] = stacks.at(sequence[i]).top();
        }
        return result;
    };

    std::cout << "Code one by one: " << getResult(sequence, stacks) << std::endl;

    stacks = inputStacks;
    for (const auto& instruction : instructions) {
        auto& from = stacks[instruction.from];
        auto& to = stacks[instruction.to];
        std::stack<char> take;
        for (std::size_t i = 0; i < instruction.count; ++i) {
            auto tmp = from.top();
            from.pop();
            take.push(tmp);
        }
        for (std::size_t i = 0; i < instruction.count; ++i) {
            auto tmp = take.top();
            take.pop();
            to.push(tmp);
        }
    }

    std::cout << "Code all at once: " << getResult(sequence, stacks) << std::endl;

    return 0;
}