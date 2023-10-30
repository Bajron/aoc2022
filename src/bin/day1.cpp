#include <algorithm>
#include <iostream>
#include <numeric>
#include <ranges>
#include <string>
#include <vector>

int main() {
    std::vector<std::vector<int>> elves;
    {
        std::vector<int> current;
        for (std::string line; std::getline(std::cin, line);) {
            if (line.empty()) {
                elves.push_back(current);
                current.clear();
            } else {
                current.push_back(std::stoi(line));
            }
        }
        if (!current.empty()) {
            elves.push_back(std::move(current));
        }
    }

    std::clog << "Elves: " << elves.size() << std::endl;
    std::clog << "Last elf has: " << elves.back().size() << std::endl;
    {
        std::vector<int> sums;
        sums.reserve(elves.size());
        std::transform(std::begin(elves), std::end(elves), std::back_inserter(sums),
                       [](const auto& a) { return std::accumulate(std::begin(a), std::end(a), 0, std::plus()); });

        const auto max = std::max_element(sums.begin(), sums.end());
        std::cout << "Max: " << *max << std::endl;

        std::sort(std::begin(sums), std::end(sums));

        const auto best3sum =
            std::accumulate(sums.rbegin(), sums.rbegin() + std::min<std::size_t>(sums.size(), 3), 0, std::plus{});
        std::cout << "Max 3: " << best3sum << std::endl;
    }
    {
        const auto v = std::views::transform(
            elves, [](const auto& a) { return std::accumulate(std::begin(a), std::end(a), 0, std::plus()); });
        std::vector<int> sums{v.begin(), v.end()};
        const auto max = std::ranges::max_element(sums);
        std::cout << "Max: " << *max << std::endl;

        std::sort(std::begin(sums), std::end(sums));
        const auto first3 = std::views::reverse(sums) | std::views::take(3);
        const auto best3sum = std::accumulate(first3.begin(), first3.end(), 0, std::plus{});
        std::cout << "Max 3: " << best3sum << std::endl;
    }
}