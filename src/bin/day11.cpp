#include <algorithm>
#include <exception>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

struct Monkey {
    std::size_t id;

    std::vector<std::size_t> items;

    std::function<std::size_t(std::size_t)> fun;

    std::size_t testDiv;
    std::size_t onTrue;
    std::size_t onFalse;

    std::size_t inspected = 0;
};

int _main() {
    std::vector<Monkey> monkeysInitial;
    for (std::string line; std::getline(std::cin, line);) {
        if (line.starts_with("Monkey")) {
            Monkey monkey;

            std::string str;
            std::istringstream iss{line};
            iss >> str >> str;
            monkey.id = std::stoul(str);

            std::getline(std::cin, line);
            std::ranges::replace(line, ',', ' ');
            std::istringstream{line}.swap(iss);
            iss >> str >> str;
            std::size_t number;
            while (iss >> number) {
                monkey.items.push_back(number);
            }

            std::getline(std::cin, line);
            std::istringstream{line}.swap(iss);
            iss >> str >> str >> str >> str;
            if (str != "old") throw std::runtime_error("expected \"old\", got " + str);

            std::string rhs;
            iss >> str >> rhs;
            if (rhs == "old") {
                if (str == "*")
                    monkey.fun = [](const auto a) { return a * a; };
                else if (str == "+")
                    monkey.fun = [](const auto a) { return a + a; };
                else
                    throw std::runtime_error("Unexpected operation " + str);
            } else {
                std::size_t argument = std::stoul(rhs);
                if (str == "*")
                    monkey.fun = [argument](const auto a) { return a * argument; };
                else if (str == "+")
                    monkey.fun = [argument](const auto a) { return a + argument; };
                else
                    throw std::runtime_error("Unexpected operation " + str);
            }

            std::getline(std::cin, line);
            std::istringstream{line}.swap(iss);
            iss >> str >> str >> str;
            iss >> monkey.testDiv;

            std::getline(std::cin, line);
            std::istringstream{line}.swap(iss);
            iss >> str >> str >> str >> str >> str >> monkey.onTrue;

            std::getline(std::cin, line);
            std::istringstream{line}.swap(iss);
            iss >> str >> str >> str >> str >> str >> monkey.onFalse;

            monkeysInitial.emplace_back(std::move(monkey));
        } else if (line.empty())
            continue;
        else
            throw std::runtime_error("Unexpected line: " + line);
    }

    std::clog << " -- Initial -- " << std::endl;
    for (const auto& m : monkeysInitial) {
        std::clog << "Monkey " << m.id << " has ";
        std::ranges::copy(m.items, std::ostream_iterator<std::size_t>(std::clog, ", "));
        std::clog << std::endl;
    }
    std::clog << std::endl;

    auto monkeys = monkeysInitial;
    for (int round = 0; round < 20; ++round) {
        for (auto& monkey : monkeys) {
            auto items = monkey.items;
            monkey.items.clear();
            for (auto i : items) {
                i = monkey.fun(i) / 3;
                if (i % monkey.testDiv == 0) {
                    monkeys[monkey.onTrue].items.push_back(i);
                } else {
                    monkeys[monkey.onFalse].items.push_back(i);
                }
            }
            monkey.inspected += items.size();
        }
    }

    const auto showResult = [](const auto& monkeys) {
        std::clog << " -- Final -- " << std::endl;
        for (const auto& m : monkeys) {
            std::clog << "Monkey " << m.id << " has ";
            std::ranges::copy(m.items, std::ostream_iterator<std::size_t>(std::clog, ", "));
            std::clog << std::endl;
        }
        std::clog << std::endl;

        for (const auto& m : monkeys) {
            std::clog << "Monkey " << m.id << " inspected " << m.inspected << std::endl;
        }

        auto values = monkeys | std::views::transform([](const auto& m) { return m.inspected; });
        auto inspected = std::vector<std::size_t>{values.begin(), values.end()};
        std::ranges::sort(inspected);

        const auto result = inspected.back() * inspected[inspected.size() - 2];
        std::cout << "Result: " << result << std::endl;
    };

    showResult(monkeys);

    auto divChecks = monkeysInitial | std::views::transform([](const auto& m) { return m.testDiv; });
    const auto mod = std::accumulate(divChecks.begin(), divChecks.end(), 1ul, std::multiplies{});

    std::clog << std::endl;
    std::clog << "Mod is " << mod << std::endl;
    std::clog << "Mod*mod is " << mod * mod << std::endl;

    auto monkeys2 = monkeysInitial;
    for (int round = 0; round < 10000; ++round) {
        for (auto& monkey : monkeys2) {
            auto items = monkey.items;
            monkey.items.clear();
            for (auto i : items) {
                i = monkey.fun(i) % mod;
                if (i % monkey.testDiv == 0) {
                    monkeys2[monkey.onTrue].items.push_back(i);
                } else {
                    monkeys2[monkey.onFalse].items.push_back(i);
                }
            }
            monkey.inspected += items.size();
        }
    }
    showResult(monkeys2);

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