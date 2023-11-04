#include <iostream>
#include <optional>
#include <string>
#include <unordered_set>

int main() {
    std::string input;
    std::getline(std::cin, input);

    // Eh... c++23 https://en.cppreference.com/w/cpp/ranges/slide_view
    const auto getMarker = [](const auto& input, const std::size_t length) {
        auto marker = std::optional<decltype(input.end())>{};
        if (input.size() < length) return marker;
        for (auto it = input.begin(), et = input.begin() + length; et < input.end(); ++it, ++et) {
            if (std::unordered_set(it, et).size() == length) {
                marker = et;
                break;
            }
        }
        return marker;
    };

    const auto m4 = getMarker(input, 4);
    if (m4) {
        std::cout << "Marker#4 end at: " << std::distance(input.cbegin(), *m4) << std::endl;
    }
    const auto m14 = getMarker(input, 14);
    if (m14) {
        std::cout << "Marker#14 end at: " << std::distance(input.cbegin(), *m14) << std::endl;
    }
    return 0;
}