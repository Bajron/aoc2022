#include <algorithm>
#include <exception>
#include <iostream>
#include <iterator>
#include <ranges>
#include <variant>
#include <vector>

template <typename T>
using PacketSelfDefinition = std::variant<int, std::vector<T>>;

template <template <typename> typename K>
struct FixCombinator : K<FixCombinator<K>> {
    using K<FixCombinator>::K;
};

using Packet = FixCombinator<PacketSelfDefinition>;

std::istream& operator>>(std::istream& is, Packet& p) {
    is >> std::ws;
    if (is.peek() == '[') {
        is.ignore(1, '[') >> std::ws;
        auto& v = p.emplace<1>();
        while (is.peek() != ']') {
            Packet el;
            is >> el >> std::ws;
            v.push_back(std::move(el));
            if (is.peek() == ',') is.ignore(1, ',');
        }
        is.ignore(1, ']');
    } else {
        is >> p.emplace<0>();
    }
    return is;
}

std::ostream& operator<<(std::ostream& os, const Packet& p) {
    if (p.index() == 0) {
        os << std::get<0>(p);
    } else {
        os << '[';
        const auto& v = std::get<1>(p);
        if (!v.empty()) os << v[0];
        for (const auto& e : v | std::views::drop(1)) {
            os << ',' << e;
        }
        os << ']';
    }
    return os;
}

std::strong_ordering operator<=>(const Packet& lhs, const Packet& rhs) {
    if (lhs.index() != rhs.index()) {
        if (lhs.index() == 0)
            return std::vector{Packet{std::get<0>(lhs)}} <=> rhs;
        else
            return lhs <=> std::vector{Packet{std::get<0>(rhs)}};
    }

    if (lhs.index() == 0) {
        return std::get<0>(lhs) <=> std::get<0>(rhs);
    }

    const auto& lv = std::get<1>(lhs);
    const auto& rv = std::get<1>(rhs);
    auto lt = lv.begin(), rt = rv.begin();
    for (; lt < lv.end() && rt < rv.end(); ++lt, ++rt) {
        const auto v = *lt <=> *rt;
        if (v != 0) return v;
    }
    if (lt == lv.end() && rt == rv.end()) return std::strong_ordering::equal;
    return lt == lv.end() ? std::strong_ordering::less : std::strong_ordering::greater;
}

std::string to_string(const std::strong_ordering& s) {
    if (std::strong_ordering::equal == s) return "equal";
    if (std::strong_ordering::less == s) return "less";
    if (std::strong_ordering::greater == s) return "greater";
    // std::unreachable();
    return "<=>";
}

int _main() {
    std::vector<std::pair<Packet, Packet>> input;
    Packet left, right;
    while (std::cin >> left >> right) {
        input.emplace_back(left, right);
    }

    std::clog << "Pairs: " << input.size() << std::endl;
    if (input.size() < 10) {
        for (const auto& p : input) {
            std::clog << p.first << " -- " << p.second << std::endl;
            std::clog << to_string(p.first <=> p.second) << '\n' << std::endl;
        }
    }

    std::size_t interestingSum = 0;
    for (std::size_t i = 0; i < input.size(); ++i) {
        const auto& p = input[i];
        if ((p.first <=> p.second) == std::strong_ordering::less) interestingSum += (i + 1);
    }

    std::cout << "Sum: " << interestingSum << std::endl;

    std::vector<Packet> packets;
    packets.reserve(input.size() * 2 + 2);
    for (const auto& i : input) {
        packets.push_back(i.first);
        packets.push_back(i.second);
    }

    std::clog << "Packets: " << packets.size() << std::endl;

    const auto two = Packet{std::vector{Packet{std::vector{Packet{2}}}}};
    const auto six = Packet{std::vector{Packet{std::vector{Packet{6}}}}};
    packets.push_back(two);
    packets.push_back(six);

    std::clog << "Added " << two << " and " << six << std::endl;
    std::clog << "Packets: " << packets.size() << std::endl;

    std::ranges::sort(packets);
    const auto i_two = std::distance(packets.begin(), std::ranges::find(packets, two));
    const auto i_six = std::distance(packets.begin(), std::ranges::find(packets, six));

    if (packets.size() < 20) {
        std::ranges::copy(packets, std::ostream_iterator<Packet>(std::clog, "\n"));
    }

    std::clog << "i_two " << i_two << std::endl;
    std::clog << "i_six " << i_six << std::endl;

    std::cout << "Code: " << (i_two + 1) * (i_six + 1) << std::endl;

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