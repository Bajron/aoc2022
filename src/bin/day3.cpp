#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <ranges>
#include <set>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

int score(const char ch) {
    if (std::islower(ch)) {
        return ch - 'a' + 1;
    } else {
        return ch - 'A' + 27;
    }
}

int main() {
    std::vector<std::string> input;
    for (std::string line; std::getline(std::cin, line);) {
        input.push_back(line);
    }

    std::ranges::transform_view tv(input, [](const auto& s) {
        const auto left = std::string_view{s.begin(), s.begin() + s.size() / 2};
        const auto right = std::string_view{s.begin() + s.size() / 2, s.end()};

        const auto lhs = std::unordered_set<char>{left.begin(), left.end()};
        const auto rhs = std::unordered_set<char>{right.begin(), right.end()};

        auto common_view = lhs | std ::views::filter([&rhs](const auto ch) { return rhs.contains(ch); });
        return std::accumulate(common_view.begin(), common_view.end(), 0,
                               [](const auto a, const auto ch) { return a + score(ch); });
    });

    const auto result = std::accumulate(tv.begin(), tv.end(), 0, std::plus{});
    std::cout << "Result: " << result << std::endl;

    // https://en.cppreference.com/w/cpp/ranges/chunk_view would be nice, but its C++23
    int group_score = 0;
    const auto set_from_string = [](const auto& s) { return std::set(s.begin(), s.end()); };
    for (auto it = input.cbegin(); it < input.cend() && it + 2 < input.cend(); it += 3) {
        const auto a = set_from_string(*it);
        const auto b = set_from_string(*(it + 1));
        const auto c = set_from_string(*(it + 2));

        std::set<char> ab;
        std::ranges::set_intersection(a, b, std::inserter(ab, ab.end()));
        std::set<char> abc;
        std::ranges::set_intersection(ab, c, std::inserter(abc, abc.end()));

        group_score +=
            std::accumulate(abc.begin(), abc.end(), 0, [](const auto a, const auto ch) { return a + score(ch); });
    }

    std::cout << "Group score: " << group_score << std::endl;

    return 0;
}