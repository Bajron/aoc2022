#include <exception>
#include <iostream>
#include <map>
#include <ranges>
#include <string>
#include <tuple>
#include <vector>

using Brick = std::vector<std::string>;

using SimulationState = std::tuple<int, int, std::string>;

struct Info {
    long long iteration;
    std::size_t height;
};

std::ostream& operator<<(std::ostream& os, const Info& i) {
    os << "Info{iteration=" << i.iteration << ", height=" << i.height << "}";
    return os;
}

int _main() {
    std::string flow;
    std::getline(std::cin, flow);

    const auto b1 = Brick{"####"};
    const auto b2 = Brick{
        ".#.",
        "###",
        ".#.",
    };

    const auto b3 = Brick{
        "..#",
        "..#",
        "###",
    };
    const auto b4 = Brick{
        "#",
        "#",
        "#",
        "#",
    };
    const auto b5 = Brick{
        "##",
        "##",
    };

    std::vector<Brick> bricks{b1, b2, b3, b4, b5};

    const auto empty = std::string{"#.......#"};
    const auto floor = std::string{"#########"};
    std::vector<std::string> state;
    state.push_back(floor);

    const auto showState = [&]() {
        for (const auto& l : state | std::views::reverse) {
            std::clog << l << std::endl;
        }
        std::clog << std::endl;
    };

    std::multimap<SimulationState, Info> appeared;

    // static constexpr auto target = 2022;
    static constexpr auto target = 1000000000000;

    long long skipped = 0;
    long long heightSkipped = 0;
    int j = 0;
    for (auto i = 0ll; i < target; ++i) {
        const auto& b = bricks[i % bricks.size()];
        const auto H = b.size();
        const auto W = b.front().size();

        auto ss = state | std::views::reverse | std::views::take(8) | std::views::join;
        const auto simState =
            SimulationState{static_cast<int>(i % bricks.size()), j, std::string{ss.begin(), ss.end()}};

        // std::clog << std::string{ss.begin(), ss.end()} << std::endl;

        appeared.insert({simState, {i, state.size()}});
        if (appeared.count(simState) > 2) {
            std::clog << "got it " << i << std::endl;
            auto eq = appeared.equal_range(simState);
            auto it = eq.first;
            std::clog << it->second << std::endl;
            const auto first = ++it;
            std::clog << it->second << std::endl;
            const auto second = ++it;
            std::clog << it->second << std::endl;
            const auto f = first->second;
            const auto s = second->second;
            const auto c = s.iteration - f.iteration;
            const auto h = s.height - f.height;
            std::clog << " cycle is " << c << " / " << h << std::endl;
            if (skipped == 0) {
                const auto futureCycles = (target - i) / c;
                skipped = futureCycles * c;
                heightSkipped = futureCycles * h;
                i += skipped;
            }
        }

        int x = 3;
        int y = state.size() + 3;
        state.resize(y + b.size(), empty);

        // std::clog << x << ", " << y << std::endl;

        bool canMove = true;
        while (canMove) {
            const auto m = flow[j];
            j = (j + 1) % flow.size();

            if (m == '<') {
                canMove = true;
                for (int h = 0; h < H; ++h) {
                    for (int w = 0; w < W; ++w) {
                        if (b[h][w] != '#') continue;
                        if (state[y + H - 1 - h][x + w - 1] == '#') {
                            canMove = false;
                            break;
                        }
                    }
                }
                if (canMove) --x;
            } else if (m == '>') {
                canMove = true;
                for (int h = 0; h < H; ++h) {
                    for (int w = 0; w < W; ++w) {
                        if (b[h][w] != '#') continue;
                        if (state[y + H - 1 - h][x + w + 1] == '#') {
                            canMove = false;
                            break;
                        }
                    }
                }
                if (canMove) ++x;
            }

            canMove = true;
            for (int h = 0; h < H; ++h) {
                for (int w = 0; w < W; ++w) {
                    if (b[h][w] != '#') continue;
                    if (state[y - 1 + H - 1 - h][x + w] == '#') {
                        canMove = false;
                        break;
                    }
                }
            }
            if (canMove) {
                --y;
            } else {
                for (int h = 0; h < H; ++h) {
                    for (int w = 0; w < W; ++w) {
                        if (b[h][w] != '#') continue;
                        state[y + H - 1 - h][x + w] = '#';
                    }
                }
            }

            // std::clog << x << ", " << y << std::endl;
        }
        while (state.back() == empty) {
            state.pop_back();
        }
    }

    // showState();
    std::cout << "Height: " << state.size() - 1 + heightSkipped << std::endl;

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