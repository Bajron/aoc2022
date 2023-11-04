#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct Interval {
    int low;
    int high;

    bool isContained(const Interval& other) const { return other.low <= low && high <= other.high; }

    bool overlap(const Interval& other) const {
        return low <= other.low && other.low <= high || low <= other.high && other.high <= high ||
               this->isContained(other);
    }

    static Interval fromString(const std::string_view& s) {
        const auto d = s.find('-');
        // What a pity there is no std::stoi for a string_view
        return Interval{std::stoi(std::string{s.begin(), s.begin() + d}),
                        std::stoi(std::string{s.begin() + d + 1, s.end()})};
    }
};

int main() {
    std::vector<std::pair<Interval, Interval>> input;

    for (std::string line; std::getline(std::cin, line);) {
        std::istringstream iss{line};
        std::vector<Interval> intervals;
        for (std::string interval; std::getline(iss, interval, ',');) {
            intervals.push_back(Interval::fromString(interval));
        }
        input.emplace_back(std::move(intervals.at(0)), std::move(intervals.at(1)));
    }

    const auto containedCount = std::ranges::count_if(
        input, [](const auto& p) { return p.first.isContained(p.second) || p.second.isContained(p.first); });

    std::cout << "Contained: " << containedCount << std::endl;

    const auto overlapCount = std::ranges::count_if(input, [](const auto& p) { return p.first.overlap(p.second); });

    std::cout << "Overlap: " << overlapCount << std::endl;

    return 0;
}