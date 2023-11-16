#include <algorithm>
#include <array>
#include <exception>
#include <iostream>
#include <iterator>
#include <ranges>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>

struct Valve {
    std::string name;
    std::size_t flow;
    std::vector<std::string> connections;
};

std::ostream& operator<<(std::ostream& os, const Valve& v) {
    os << v.name << " flow=" << v.flow << " connects: ";
    std::ranges::copy(v.connections, std::ostream_iterator<std::string>(os, ", "));
    return os;
}

struct FlowPotential {
    int esitmatedFlow;
    int minutesLeft;

    bool operator<(const FlowPotential& rhs) const {
        return esitmatedFlow < rhs.esitmatedFlow || esitmatedFlow == rhs.esitmatedFlow && minutesLeft < rhs.minutesLeft;
    }
};

static constexpr auto minuteLimit = 30;

struct Context {
    Context(const std::unordered_map<std::string, Valve>& g,
            const std::unordered_map<std::string, std::unordered_map<std::string, int>>& d)
        : graph{g}, distance{d} {
        // Treat 0 flow valves as already open
        auto z = graph | std::views::values | std::views::filter([](const auto& v) { return v.flow == 0; }) |
                 std::views::transform([](const auto& v) { return std::make_pair(v.name, 0); });
        open = std::unordered_map<std::string, int>{z.begin(), z.end()};

        auto a = graph | std::views::values | std::views::filter([](const auto& v) { return v.flow > 0; }) |
                 std::views::transform([](const auto& v) { return v.name; });
        available = std::unordered_set<std::string>(a.begin(), a.end());

        showState();
    }

    const std::unordered_map<std::string, Valve>& graph;
    const std::unordered_map<std::string, std::unordered_map<std::string, int>>& distance;
    std::unordered_map<std::string, int> open;
    std::unordered_set<std::string> available;

    int best = 0;

    int bestCanGet(int minutes) const {
        std::vector<std::size_t> flows;
        flows.reserve(available.size());
        for (const auto& a : available) {
            flows.push_back(graph.at(a).flow);
        }
        std::sort(flows.begin(), flows.end());

        int sum = 0;
        for (auto it = flows.rbegin(); it < flows.rend() && minutes > 0; ++it) {
            sum += --minutes * *it;
        }
        return sum;
    }

    void showState() const {
        std::clog << "Open:";
        for (const auto& [k, v] : open) {
            std::clog << " " << k;
        }
        std::clog << std::endl;

        std::clog << "Available:";
        for (const auto& v : available) {
            std::clog << " " << v;
        }
        std::clog << std::endl;
    }

    void showWhenOpened() const {
        for (const auto [k, m] : open) {
            std::clog << k << ":" << m << " ";
        }
        std::clog << std::endl;
    }
};

void search(const std::string& current, int minutes, int flowRate, int cumFlow, int expectedTotalFlow, Context& ctx) {
    if (minutes == 0 || ctx.open.size() == ctx.graph.size()) {
        if (expectedTotalFlow > ctx.best) {
            ctx.best = expectedTotalFlow;
            std::clog << "finish " << expectedTotalFlow << " " << cumFlow + minutes * flowRate << std::endl;
            // ctx.showWhenOpened();
        }
        return;
    }

    const auto bestToGet = expectedTotalFlow + ctx.bestCanGet(minutes);
    if (bestToGet < ctx.best) {
        return;
    }

    const auto& cur = ctx.graph.at(current);
    if (!ctx.open.contains(current)) {
        ctx.open.insert({current, minutes});
        ctx.available.erase(current);

        const auto newTotalFlow = expectedTotalFlow + (minutes - 1) * cur.flow;
        search(current, minutes - 1, flowRate + cur.flow, cumFlow + flowRate, newTotalFlow, ctx);

        ctx.open.erase(current);
        ctx.available.insert(current);
    }

    // for (const auto& n : cur.connections) {
    //     search(n, minutes - 1, flowRate, nextCumFlow, expectedTotalFlow, ctx);
    // }
    const auto availableNodes = ctx.available;
    bool searched = false;
    for (const auto& a : availableNodes) {
        const auto d = ctx.distance.at(current).at(a);
        if ((minutes - d) < 0) continue;

        searched = true;
        search(a, minutes - d, flowRate, cumFlow + d * flowRate, expectedTotalFlow, ctx);
    }
    if (!searched) {
        search(current, 0, flowRate, cumFlow + minutes * flowRate, expectedTotalFlow, ctx);
    }
}

int _main() {
    std::vector<Valve> valves;

    std::regex re{"Valve ([A-Z]{2}) has flow rate=([0-9]+); tunnels? leads? to valves? (([A-Z]{2})(, [A-Z]{2})*)"};
    for (std::string line; std::getline(std::cin, line);) {
        std::smatch matches;
        if (std::regex_match(line, matches, re)) {
            auto r = std::ranges::subrange(matches[3].first, matches[3].second) | std::views::split(std::string{", "}) |
                     std::views::transform([](const auto& r) {
                         return std::string{r.begin(), r.end()};
                     });
            valves.push_back(Valve{matches[1].str(), std::stoul(matches[2].str()), {r.begin(), r.end()}});
        }
    }

    std::clog << "Valves: " << valves.size() << std::endl;
    for (const auto& v : valves) {
        std::clog << v << std::endl;
    }

    auto g = valves | std::views::transform([](const auto& v) { return std::make_pair(v.name, v); });
    std::unordered_map<std::string, Valve> graph{g.begin(), g.end()};

    std::unordered_map<std::string, std::unordered_map<std::string, int>> distances;
    for (const auto& v : valves) {
        std::unordered_map<std::string, int> valveDistances;

        std::vector<std::string> current = v.connections;
        std::vector<std::string> next;
        int distance = 1;
        while (!current.empty()) {
            next.clear();
            for (const auto& n : current) {
                if (!valveDistances.contains(n)) {
                    valveDistances[n] = distance;
                    next.insert(next.end(), graph[n].connections.begin(), graph[n].connections.end());
                }
            }
            ++distance;
            std::swap(next, current);
        }

        distances[v.name] = std::move(valveDistances);
    }

    std::clog << "Got distances" << std::endl;
    if (distances.size() < 12) {
        for (const auto& [k, d] : distances) {
            std::clog << k << ":  ";
            for (const auto& [n, k] : d) {
                std::clog << " [" << n << ":" << k << "]";
            }
            std::clog << std::endl;
        }
    }

    Context ctx{graph, distances};
    search("AA", minuteLimit, 0, 0, 0, ctx);

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