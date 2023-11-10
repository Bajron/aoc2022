#include <algorithm>
#include <cstdint>
#include <exception>
#include <iostream>
#include <ranges>
#include <regex>
#include <string>
#include <unordered_set>

struct Point {
    int x;
    int y;
};

bool operator==(const Point& lhs, const Point& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }

template <>
struct std::hash<Point> {
    std::size_t operator()(const Point& s) const noexcept {
        const auto h1 = std::hash<int>{}(s.x);
        const auto h2 = std::hash<int>{}(s.y);
        return h1 ^ (h2 << 1);
    }
};

int dist(const Point& a, const Point& b) { return std::abs(a.x - b.x) + std::abs(a.y - b.y); }

struct Sensor {
    Sensor(Point l, Point c) : location(std::move(l)), closest(std::move(c)), d(dist(this->location, this->closest)) {}

    const Point location;
    const Point closest;
    const int d;

    bool inRange(const Point& p) const { return dist(p, location) <= d; }

    std::vector<Point> outBorder() const {
        std::vector<Point> b;
        int o = d + 1;
        b.reserve(2 * (2 * o + 1));
        for (int xd = -o; xd <= o; ++xd) {
            const int yd = o - std::abs(xd);
            b.push_back({location.x + xd, location.y + yd});
            b.push_back({location.x + xd, location.y - yd});
        }
        return b;
    }
};

int _main() {
    std::regex re{"Sensor at x=(-?[0-9]+), y=(-?[0-9]+): closest beacon is at x=(-?[0-9]+), y=(-?[0-9]+)"};
    std::vector<Sensor> sensors;
    for (std::string line; std::getline(std::cin, line);) {
        std::smatch matches;
        if (std::regex_search(line, matches, re)) {
            sensors.emplace_back(Point{std::stoi(matches[1].str()), std::stoi(matches[2].str())},
                                 Point{std::stoi(matches[3].str()), std::stoi(matches[4].str())});
        }
    }

    std::clog << "Sensors: " << sensors.size() << std::endl;
    std::unordered_set<Point> points;
    std::ranges::for_each(sensors, [&points](const auto& s) {
        points.insert(s.location);
        points.insert(s.closest);
    });

    const auto [minX, maxX] = std::ranges::minmax(points | std::views::transform([](const auto& p) { return p.x; }));
    const auto [minY, maxY] = std::ranges::minmax(points | std::views::transform([](const auto& p) { return p.y; }));

    const auto width = maxX - minX + 1;
    const auto height = maxY - minY + 1;

    const auto from = minX - height;
    const auto to = maxX + height;

    int count10 = 0;
    {
        int y = 10;
        for (int x = from; x < to; ++x) {
            const auto p = Point{x, y};
            if (std::ranges::any_of(sensors, [&](const auto& s) { return s.inRange(p); }) && !points.contains(p)) {
                ++count10;
            }
        }
        std::clog << "Count " << y << ": " << count10 << std::endl;
    }

    int count2000000 = 0;
    {
        int y = 2000000;
        for (int x = from; x < to; ++x) {
            const auto p = Point{x, y};
            if (std::ranges::any_of(sensors, [&](const auto& s) { return s.inRange(p); }) && !points.contains(p)) {
                ++count2000000;
            }
        }
        std::clog << "Count " << y << ": " << count2000000 << std::endl;
    }

    if (count2000000 == 0) {
        constexpr auto limit = 20;
        for (int y = 0; y <= limit; ++y) {
            for (int x = 0; x <= limit; ++x) {
                const auto p = Point{x, y};
                if (std::ranges::none_of(sensors, [&](const auto& s) { return s.inRange(p); }) && !points.contains(p)) {
                    std::clog << "Limit 20: x=" << x << " y=" << y << std::endl;
                    std::cout << "20  " << (std::int64_t{4000000} * x + y) << std::endl;
                }
            }
        }

        for (const auto& s : sensors) {
            for (const auto& p : s.outBorder()) {
                if (p.x < 0 || p.x > limit || p.y < 0 || p.y > limit) continue;
                if (std::ranges::none_of(sensors, [&](const auto& s) { return s.inRange(p); }) && !points.contains(p)) {
                    std::clog << "Limit " << limit << ": x=" << p.x << " y=" << p.y << std::endl;
                    std::cout << limit << "  " << (std::int64_t{4000000} * p.x + p.y) << std::endl;
                }
            }
        }
    }

    if (count2000000 > 0) {
        constexpr auto limit = 4000000;
        // for (int y = 0; y <= limit; ++y) {
        //     for (int x = 0; x <= limit; ++x) {
        //         const auto p = Point{x, y};
        //         if (std::ranges::none_of(sensors, [&](const auto& s) { return s.inRange(p); }) && !points.contains(p)) {
        //             std::clog << "Limit 4000000: x=" << x << " y=" << y << std::endl;
        //             std::cout << "4000000  " << (std::int64_t{4000000} * x + y) << std::endl;
        //         }
        //     }
        //     if (y % 1000 == 0) std::clog << y << std::endl;
        // }


        for (const auto& s : sensors) {
            for (const auto& p : s.outBorder()) {
                if (p.x < 0 || p.x > limit || p.y < 0 || p.y > limit) continue;
                if (std::ranges::none_of(sensors, [&](const auto& s) { return s.inRange(p); }) && !points.contains(p)) {
                    std::clog << "Limit " << limit << ": x=" << p.x << " y=" << p.y << std::endl;
                    std::cout << limit << "  " << (std::int64_t{4000000} * p.x + p.y) << std::endl;
                }
            }
        }
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