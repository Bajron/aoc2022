#include <iostream>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

enum class Sign { Rock = 0, Paper = 1, Scissors = 2 };
enum class Result { Win = 6, Draw = 3, Lose = 0 };

static const Result result[3][3] = {{Result::Draw, Result::Lose, Result::Win},
                                    {Result::Win, Result::Draw, Result::Lose},
                                    {Result::Lose, Result::Win, Result::Draw}};

constexpr int getValue(const Sign s) { return static_cast<int>(s) + 1; }

int myResult(const Sign me, const Sign opponent) {
    return static_cast<int>(result[static_cast<int>(me)][static_cast<int>(opponent)]) + getValue(me);
}

constexpr auto opposite(const Result r) {
    return (r == Result::Draw) ? r : ((r == Result::Lose) ? Result::Win : Result::Lose);
}

Sign mySign(const Result myStrategy, const Sign opponent) {
    const auto opponentResult = opposite(myStrategy);
    const auto& opponentRow = result[static_cast<int>(opponent)];
    const auto it = std::find(std::begin(opponentRow), std::end(opponentRow), opponentResult);
    return static_cast<Sign>(std::distance(std::begin(opponentRow), it));
}

struct RoundMoves {
    Sign opponent;
    Sign me;
};

struct RoundStrategy {
    Sign opponent;
    Result myStrategy;
};

int main() {
    const std::unordered_map<std::string, Sign> translation{{"A", Sign::Rock},     {"B", Sign::Paper},
                                                            {"C", Sign::Scissors}, {"X", Sign::Rock},
                                                            {"Y", Sign::Paper},    {"Z", Sign::Scissors}};
    const std::unordered_map<std::string, Result> resultTranslation{
        {"X", Result::Lose}, {"Y", Result::Draw}, {"Z", Result::Win}};

    std::vector<RoundMoves> inputMoves;
    std::vector<RoundStrategy> inputStrategies;
    {
        std::string opponent, me;
        for (std::string line; std::getline(std::cin, line);) {
            std::istringstream iss{line};
            iss >> opponent >> me;
            const auto opponentMove = translation.at(opponent);
            inputMoves.push_back(RoundMoves{opponentMove, translation.at(me)});
            inputStrategies.push_back(RoundStrategy{opponentMove, resultTranslation.at(me)});
        }
    }
    std::clog << "Input size: " << inputMoves.size() << std::endl;

    std::ranges::transform_view myMovesResults(inputMoves, [](const auto& e) { return myResult(e.me, e.opponent); });
    const auto movesResult = std::accumulate(myMovesResults.begin(), myMovesResults.end(), 0, std::plus{});

    std::cout << "Result moves: " << movesResult << std::endl;

    std::ranges::transform_view myStrategiesResults(
        inputStrategies, [](const auto& e) { return myResult(mySign(e.myStrategy, e.opponent), e.opponent); });
    const auto strategiesResult =
        std::accumulate(myStrategiesResults.begin(), myStrategiesResults.end(), 0, std::plus{});

    std::cout << "Result strategies: " << strategiesResult << std::endl;

    return 0;
}