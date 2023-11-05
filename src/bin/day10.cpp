#include <algorithm>
#include <exception>
#include <iostream>
#include <optional>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

struct State {
    int cycle = 1;
    int value = 1;

    bool operator<(const State& rhs) const { return cycle < rhs.cycle; }
};

int _main() {
    std::vector<std::optional<int>> addOps;
    for (std::string line; std::getline(std::cin, line);) {
        if (line == "noop") {
            addOps.push_back({});
        } else {
            std::istringstream iss{line};
            std::string tmp;
            iss >> tmp >> tmp;
            addOps.push_back(std::stoi(tmp));
        }
    }
    std::vector<State> changes;
    State current{1, 1};
    changes.push_back(current);

    for (auto op : addOps) {
        if (op.has_value()) {
            current.cycle += 2;
            current.value += *op;
            changes.push_back(current);
        } else {
            ++current.cycle;
        }
    }

    std::vector checkCycles = {20, 60, 100, 140, 180, 220};

    for (const auto s : changes | std::views::take(22)) {
        std::clog << s.cycle << ": " << s.value << std::endl;
    }
    std::clog << std::endl;

    int strenghtSum = 0;
    for (auto check : checkCycles) {
        auto it = std::lower_bound(changes.begin(), changes.end(), State{check, 0});
        if (it->cycle != check) {
            --it;
        }
        const auto strenght = check * it->value;
        strenghtSum += strenght;
        std::clog << "At " << check << " value is " << it->value << ", strength is " << strenght << std::endl;
    }

    std::cout << "Check sum: " << strenghtSum << std::endl;

    State state{1, 1};
    auto change = changes.begin();

    for (int c = 1, x = 0; c <= 240; ++c, ++x) {
        if (change != changes.end() && change->cycle == c) {
            state = *change++;
        }
        if (x >= 40) {
            x = 0;
            std::cout << std::endl;
        }
        std::cout << (std::abs(x - state.value) <= 1 ? '#' : '.');
    }

    return 0;
}

int main() {
    try {
        return _main();
    } catch (const std::exception& ex) {
        std::cerr << " ** Exception: " << ex.what() << std::endl;
    } catch (...) {
        std::cerr << " ** Unknown exception" << std::endl;
    }
    return 1;
}