#include <array>
#include <exception>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

constexpr auto GUARD = std::numeric_limits<std::size_t>::max();

struct Position {
    std::size_t y = GUARD;
    std::size_t x = GUARD;

    bool operator==(const Position& rhs) const { return y == rhs.y && x == rhs.x; }
};

std::ostream& operator<<(std::ostream& out, const Position& p) {
    out << "[" << p.y << ", " << p.x << "]";
    return out;
}

struct Context {
    Context(const std::vector<std::string>& hmap, const Position& destination)
        : hmap{hmap},
          width{this->hmap.front().size()},
          height{this->hmap.size()},
          shortest{hmap.size(), std::vector(hmap.front().size(), std::numeric_limits<int>::max())},
          destination{destination} {}

    const std::vector<std::string>& hmap;
    const std::size_t width;
    const std::size_t height;
    std::vector<std::vector<int>> shortest;
    Position destination;

    std::size_t fillNeighbors(std::array<Position, 4>& neighbors, const std::size_t y, const std::size_t x) {
        std::size_t i = 0;
        if (x > 0) {
            neighbors[i++] = {y, x - 1};
        }
        if (x < width - 1) {
            neighbors[i++] = {y, x + 1};
        }
        if (y > 0) {
            neighbors[i++] = {y - 1, x};
        }
        if (y < height - 1) {
            neighbors[i++] = {y + 1, x};
        }
        return i;
    }
};

void search(const Position& current, const int steps, Context& ctx) {
    const auto [y, x] = current;
    // std::clog << " in " << y << ", " << x << std::endl;
    if (steps < ctx.shortest[y][x]) {
        ctx.shortest[y][x] = steps;
    } else {
        return;
    }
    if (current == ctx.destination) {
        return;
    }

    const auto c = ctx.hmap[y][x];

    std::array<Position, 4> neighbors;
    const auto len = ctx.fillNeighbors(neighbors, y, x);

    for (std::size_t j = 0; j < len; ++j) {
        const auto& np = neighbors[j];
        const auto nc = ctx.hmap[np.y][np.x];
        // std::clog << "considering " << np << " " << c << " -> " << nc << std::endl;
        if (nc - c <= 1) {
            search(np, steps + 1, ctx);
        }
    }
}

void searchBack(const Position& current, const int steps, Context& ctx) {
    const auto [y, x] = current;

    if (steps < ctx.shortest[y][x]) {
        ctx.shortest[y][x] = steps;
    } else {
        return;
    }
    const auto c = ctx.hmap[y][x];
    if (c == 'a') {
        return;
    }

    std::array<Position, 4> neighbors;
    const auto len = ctx.fillNeighbors(neighbors, y, x);

    for (std::size_t j = 0; j < len; ++j) {
        const auto& np = neighbors[j];
        const auto nc = ctx.hmap[np.y][np.x];
        if (nc - c >= -1) {
            searchBack(np, steps + 1, ctx);
        }
    }
}

int _main() {
    std::vector<std::string> hmap;
    for (std::string line; std::getline(std::cin, line);) {
        hmap.push_back(line);
    }

    Position start, end;
    for (std::size_t y = 0; y < hmap.size(); ++y) {
        for (std::size_t x = 0; x < hmap[y].size(); ++x) {
            auto& v = hmap[y][x];
            if (v == 'S') {
                v = 'a';
                start = {y, x};
            } else if (v == 'E') {
                v = 'z';
                end = {y, x};
            }
        }
    }

    std::clog << "Start: " << start << std::endl;
    std::clog << "End: " << end << std::endl;

    Context context{hmap, end};
    search(start, 0, context);

    std::clog << "Width: " << context.width << std::endl;
    std::clog << "Height: " << context.height << std::endl;
    std::cout << "Shortest: " << context.shortest[end.y][end.x] << std::endl;

    Context contextBack{hmap, end};
    searchBack(end, 0, contextBack);
    int shortestBack = std::numeric_limits<int>::max();
    for (std::size_t y = 0; y < hmap.size(); ++y) {
        for (std::size_t x = 0; x < hmap[y].size(); ++x) {
            auto v = hmap[y][x];
            if (v != 'a') continue;
            shortestBack = std::min(contextBack.shortest[y][x], shortestBack);
        }
    }

    std::cout << "Shortest back: " << shortestBack << std::endl;

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