#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

struct Move {
    char direction;
    std::size_t count;
};

using Position = std::tuple<int, int>;

std::ostream& operator<<(std::ostream& out, const Position& p) {
    out << "[" << std::get<0>(p) << ", " << std::get<1>(p) << "]";
    return out;
}

template <>
struct std::hash<Position> {
    std::size_t operator()(const Position& s) const noexcept {
        const auto h1 = std::hash<int>{}(std::get<0>(s));
        const auto h2 = std::hash<int>{}(std::get<1>(s));
        return h1 ^ (h2 << 1);
    }
};

Position diff(const Position& from, const Position& to) {
    using std::get;
    return {get<0>(to) - get<0>(from), get<1>(to) - get<1>(from)};
}

Position add(const Position& a, const Position& b) {
    using std::get;
    return {get<0>(a) + get<0>(b), get<1>(a) + get<1>(b)};
}

template <typename T>
int signum(T val) {
    return (T(0) < val) - (val < T(0));
}

Position damp(const Position& d) {
    using std::get;
    return {signum(get<0>(d)), signum(get<1>(d))};
}

int _main() {
    std::vector<Move> input;
    for (std::string line; std::getline(std::cin, line);) {
        Move m;
        std::istringstream iss(line);
        iss >> m.direction >> m.count;
        input.push_back(std::move(m));
    }

    // The task says 10 knots, but it is 1 head + 9 tails
    constexpr auto TAIL_COUNT = 9;

    std::unordered_set<Position> visited{};
    Position head{0, 0};
    std::vector<Position> tails(TAIL_COUNT, Position{0, 0});
    visited.insert(tails.back());

    const auto updateTails = [&]() {
        Position currentHead = head;
        for (auto& tail : tails) {
            const auto d = diff(tail, currentHead);
            if (std::abs(std::get<0>(d)) > 1 || std::abs(std::get<1>(d)) > 1) {
                const auto move = damp(d);
                tail = add(tail, move);
            }
            currentHead = tail;
        }
        visited.insert(tails.back());

        // std::clog << tail << ".." << head << std::endl;
    };

    for (const auto m : input) {
        std::clog << m.direction << " " << m.count << std::endl;

        switch (m.direction) {
            case 'L':
                for (std::size_t i = 0; i < m.count; ++i) {
                    std::get<0>(head) -= 1;
                    updateTails();
                }
                break;
            case 'R':
                for (std::size_t i = 0; i < m.count; ++i) {
                    std::get<0>(head) += 1;
                    updateTails();
                }
                break;
            case 'U':
                for (std::size_t i = 0; i < m.count; ++i) {
                    std::get<1>(head) -= 1;
                    updateTails();
                }
                break;
            case 'D':
                for (std::size_t i = 0; i < m.count; ++i) {
                    std::get<1>(head) += 1;
                    updateTails();
                }
                break;
            default:
                throw std::runtime_error("Unexpected move " + m.direction);
        }
    }

    std::clog << "Tail knots " << tails.size() << std::endl;
    std::cout << "Last tail visited: " << visited.size() << std::endl;

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