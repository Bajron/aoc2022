#include <algorithm>
#include <exception>
#include <iostream>
#include <memory>
#include <numeric>
#include <ranges>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

struct Dir;

struct File {
    std::string name;
    std::size_t size = 0;
    Dir* parent = nullptr;  // to maybe add more efficient updates of size
};

struct Dir {
    std::string name;
    Dir* parent = nullptr;
    std::unordered_map<std::string, std::unique_ptr<Dir>> directories;
    std::vector<File> files;

    void gather(std::vector<const Dir*>& collection) const {
        collection.push_back(this);
        for (const auto& [n, d] : directories) {
            d->gather(collection);
        }
    }
    std::size_t calculateSize() const {
        const auto d =
            directories | std::views::values | std::views::transform([](const auto& v) { return v->calculateSize(); });
        const auto f = files | std::views::transform([](const auto& f) { return f.size; });
        // ranges::views::concat ;( in c++26 maybe
        return std::accumulate(d.begin(), d.end(), static_cast<std::size_t>(0), std::plus{}) +
               std::accumulate(f.begin(), f.end(), static_cast<std::size_t>(0), std::plus{});
    }
};

int _main() {
    auto root = Dir{"/"};
    std::stack<Dir*> stack;

    bool listingFollows = false;
    for (std::string line; std::getline(std::cin, line);) {
        if (line[0] == '$') {
            listingFollows = false;

            std::string prompt;
            std::string command;
            std::istringstream iss{line};
            iss >> prompt >> command;

            if (command == "cd") {
                std::string into;
                iss >> into;
                if (into == "/") {
                    stack.push(&root);
                } else if (into == "..") {
                    stack.pop();
                } else {
                    // NOTE: .at to make sure we've seen it before on ls
                    stack.push(stack.top()->directories.at(into).get());
                }
            } else if (command == "ls") {
                listingFollows = true;
            }
        } else if (listingFollows) {
            std::string dirOrSize, name;
            std::istringstream iss{line};
            iss >> dirOrSize >> name;
            if (dirOrSize == "dir") {
                stack.top()->directories[name] = std::make_unique<Dir>(Dir{name, stack.top()});
            } else {
                stack.top()->files.push_back(File{name, std::stoul(dirOrSize)});
            }
        }
    }

    std::vector<const Dir*> allDirectories;
    root.gather(allDirectories);
    std::clog << "All directories: " << allDirectories.size() << std::endl;
    for (const auto* d : allDirectories) {
        std::clog << d->name << ": " << d->calculateSize() << std::endl;
    }

    auto interesting = allDirectories | std::views::transform([](const auto* d) { return d->calculateSize(); }) |
                       std::views::filter([](const auto s) { return s <= 100000; });

    const auto sum = std::accumulate(interesting.begin(), interesting.end(), std::size_t{}, std::plus{});
    std::cout << "Interesting sum: " << sum << std::endl;

    constexpr auto total = 70000000;
    constexpr auto required = 30000000;

    const auto used = root.calculateSize();
    const auto available = total - used;
    auto choice = allDirectories | std::views::transform([](const auto* d) { return d->calculateSize(); }) |
                  std::views::filter([=](const auto s) { return available + s > required; });
    auto min = *std::ranges::min_element(choice);

    std::cout << "To delete: " << min << std::endl;

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