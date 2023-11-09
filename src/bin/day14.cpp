#include <algorithm>
#include <charconv>
#include <exception>
#include <iostream>
#include <iterator>
#include <ranges>
#include <string>
#include <vector>

struct Point {
    int x;
    int y;
};

std::ostream& operator<<(std::ostream& os, const Point& p) {
    os << p.x << ',' << p.y;
    return os;
}

using Line = std::vector<Point>;

void checkSettled(const std::vector<Line>& lines, const Point start, int xMin, int xMax, int yMin, int yMax) {
    constexpr auto margin = 2;
    const auto width = xMax - xMin + 1 + margin;  // add margin for easier checks
    const auto height = yMax - yMin + 1;

    auto grid = std::vector(height, std::string(width, '.'));

    const auto drawLine = [&](const Point& a, const Point& b) {
        const auto [x0, x1] = std::minmax(a.x, b.x);
        const auto [y0, y1] = std::minmax(a.y, b.y);
        for (int x = x0; x <= x1; ++x) {
            for (int y = y0; y <= y1; ++y) {
                grid[y - yMin][x - xMin + margin / 2] = '#';
            }
        }
    };

    for (const auto& l : lines) {
        for (std::size_t i = 0; i < l.size() - 1; ++i) {
            drawLine(l[i], l[i + 1]);
        }
    }

    const auto show = [&]() {
        if (width > 120) return;
        for (const auto& gl : grid) {
            std::clog << gl << std::endl;
        }
        std::clog << std::endl;
    };
    show();

    const auto drop = [&]() -> bool {
        auto pos = Point{start.x - xMin + margin / 2, start.y - yMin};
        while (pos.y < height - 1) {
            if (grid[pos.y + 1][pos.x] == '.') {
                pos.y += 1;
            } else if (grid[pos.y + 1][pos.x - 1] == '.') {
                pos.y += 1;
                pos.x -= 1;
            } else if (grid[pos.y + 1][pos.x + 1] == '.') {
                pos.y += 1;
                pos.x += 1;
            } else {
                grid[pos.y][pos.x] = 'o';
                break;
            }
        }
        // We are falling below
        return pos.y < height - 1;
    };

    std::size_t settled = 0;
    while (drop()) {
        ++settled;
    }

    show();
    std::cout << "Settled: " << settled << std::endl;
}

void checkAppeared(const std::vector<Line>& lines, const Point start, int xMin, int xMax, int yMin, int yMax) {
    const auto floorLevel = yMax + 2;
    const auto needSide = floorLevel + 2;  // margin just to be sure

    const auto nxMin = std::min(xMin, start.x - needSide);
    const auto nxMax = std::max(xMax, start.x + needSide);
    const auto nyMin = yMin;
    const auto nyMax = floorLevel;

    const auto width = nxMax - nxMin + 1;
    const auto height = nyMax - nyMin + 1;

    std::clog << "width: " << width << " height: " << height << std::endl;
    auto grid = std::vector(height, std::string(width, '.'));

    const auto drawLine = [&](const Point& a, const Point& b) {
        const auto [x0, x1] = std::minmax(a.x, b.x);
        const auto [y0, y1] = std::minmax(a.y, b.y);
        for (int x = x0; x <= x1; ++x) {
            for (int y = y0; y <= y1; ++y) {
                grid[y - nyMin][x - nxMin] = '#';
            }
        }
    };

    for (const auto& l : lines) {
        for (std::size_t i = 0; i < l.size() - 1; ++i) {
            drawLine(l[i], l[i + 1]);
        }
    }

    drawLine(Point{nxMin, floorLevel}, Point{nxMax, floorLevel});

    const auto show = [&]() {
        if (width > 120) return;
        for (const auto& gl : grid) {
            std::clog << gl << std::endl;
        }
        std::clog << std::endl;
    };
    show();

    const auto drop = [&]() -> bool {
        auto pos = Point{start.x - nxMin, start.y - nyMin};
        if (grid[pos.y][pos.x] != '.') return false;

        while (pos.y < height - 1) {
            if (grid[pos.y + 1][pos.x] == '.') {
                pos.y += 1;
            } else if (grid[pos.y + 1][pos.x - 1] == '.') {
                pos.y += 1;
                pos.x -= 1;
            } else if (grid[pos.y + 1][pos.x + 1] == '.') {
                pos.y += 1;
                pos.x += 1;
            } else {
                grid[pos.y][pos.x] = 'o';
                break;
            }
        }

        return true;
    };

    std::size_t appeared = 0;
    while (drop()) {
        ++appeared;
    }
    show();
    std::cout << "Appeared: " << appeared << std::endl;
}

int _main() {
    std::vector<Line> lines;
    for (std::string line; std::getline(std::cin, line);) {
        Line l;
        // Well... this is not great.
        // Need delim at the end.
        // Watch auto for const char* delim for string (types must match)
        for (const auto point : std::views::split(line, std::string{" -> "})) {
            const auto s = std::string_view{point.begin(), point.end()};
            const auto comma = s.find(',');
            Point p;
            // Watch out for leading spaces...
            std::from_chars(s.data(), s.data() + comma, p.x);
            std::from_chars(s.data() + comma + 1, s.data() + s.length(), p.y);
            l.push_back(std::move(p));
        }
        lines.push_back(std::move(l));
    }

    std::clog << "lines: " << lines.size() << std::endl;
    if (lines.size() < 10) {
        for (const auto& l : lines) {
            std::ranges::copy(l, std::ostream_iterator<Point>(std::clog, " -> "));
            std::clog << std::endl;
        }
    }

    const auto start = Point{500, 0};

    // Add silly line, so we get the starting point in the minmax
    lines.push_back({start});
    const auto [xMin, xMax] =
        std::ranges::minmax(lines | std::views::join | std::views::transform([](const auto& p) { return p.x; }));
    const auto [yMin, yMax] =
        std::ranges::minmax(lines | std::views::join | std::views::transform([](const auto& p) { return p.y; }));
    lines.pop_back();

    std::clog << "x: [" << xMin << ", " << xMax << "]" << std::endl;
    std::clog << "y: [" << yMin << ", " << yMax << "]" << std::endl;

    checkSettled(lines, start, xMin, xMax, yMin, yMax);

    checkAppeared(lines, start, xMin, xMax, yMin, yMax);

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