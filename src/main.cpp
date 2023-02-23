#include "BinanceClient.h"

#include <fmt/core.h>
#include <fmt/os.h>

#include <set>

auto main() -> int {
    BinanceClient binanceClient;

    for (const auto& trade : binanceClient.trades) {
        fmt::print("Cycle: {}, {}, {}\n", trade.cycle[0], trade.cycle[1], trade.cycle[2]);
        fmt::print("Profit: {}\n", trade.profit);
    }

    return 0;
}