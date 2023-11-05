#include <algorithm>
#include <exception>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

int _main() {
    const auto& grid = []() {
        std::vector<std::vector<int>> grid;
        for (std::string line; std::getline(std::cin, line);) {
            auto v = line | std::views::transform([](const char ch) { return ch - '0'; });
            grid.emplace_back(v.begin(), v.end());
        }
        return grid;
    }();

    auto visible = std::vector(grid.size(), std::vector(grid.front().size(), 0));
    for (std::size_t y = 0; y < grid.size(); ++y) {
        const auto& row = grid[y];
        auto& visRow = visible[y];

        auto highest = row.front();
        ++visRow[0];
        for (std::size_t i = 1; i < row.size(); ++i) {
            if (row[i] > highest) {
                ++visRow[i];
                highest = row[i];
            }
        }

        highest = row.back();
        ++visRow[row.size() - 1];
        for (int x = row.size() - 2; x >= 0; --x) {
            if (row[x] > highest) {
                ++visRow[x];
                highest = row[x];
            }
        }
    }

    for (std::size_t x = 0; x < grid.size(); ++x) {
        auto highest = grid[0][x];
        ++visible[0][x];
        for (std::size_t y = 1; y < grid.size(); ++y) {
            if (grid[y][x] > highest) {
                ++visible[y][x];
                highest = grid[y][x];
            }
        }

        highest = grid[grid.size() - 1][x];
        ++visible[grid.size() - 1][x];
        for (int y = grid.size() - 2; y >= 0; --y) {
            if (grid[y][x] > highest) {
                ++visible[y][x];
                highest = grid[y][x];
            }
        }
    }

    const auto flat = visible | std::views::join;
    const auto visibleCount = std::ranges::count_if(flat, [](const auto v) { return v > 0; });
    std::cout << "Visible: " << visibleCount << std::endl;

    const auto getScore = [&grid](const std::size_t y, const std::size_t x) {
        const auto source = grid[y][x];

        int left = 0;
        for (int xx = x - 1; xx >= 0; --xx) {
            ++left;
            if (grid[y][xx] >= source) break;
        }
        int right = 0;
        for (int xx = x + 1; xx < grid[y].size(); ++xx) {
            ++right;
            if (grid[y][xx] >= source) break;
        }
        int top = 0;
        for (int yy = y - 1; yy >= 0; --yy) {
            ++top;
            if (grid[yy][x] >= source) break;
        }
        int bottom = 0;
        for (int yy = y + 1; yy < grid.size(); ++yy) {
            ++bottom;
            if (grid[yy][x] >= source) break;
        }
        return left * right * top * bottom;
    };

    auto scores = std::vector(grid.size(), std::vector(grid.front().size(), 0));
    for (std::size_t y = 1; y < grid.size() - 1; ++y) {
        for (std::size_t x = 1; x < grid[y].size() - 1; ++x) {
            scores[y][x] = getScore(y, x);
        }
    }

    const auto flatScores = scores | std::views::join;
    const auto maxScore = std::ranges::max(flatScores);
    std::cout << "Max score: " << maxScore << std::endl;

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