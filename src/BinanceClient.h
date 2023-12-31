#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

struct TradingPair {
    std::string name;
    std::string baseAsset;
    std::string quoteAsset;
};

struct Trade {
    std::vector<std::string> cycle;
    double profit{0.};
};

struct TradingPairPrice {
    double bidPrice;
    double askPrice;
};

class BinanceClient {
public:
    std::set<std::string> tickers;
    std::set<std::string> symbols;
    std::map<std::string, std::vector<std::string>> connections;
    std::map<std::string, std::vector<std::string>> blacklist;
    std::vector<std::vector<std::string>> cycles;
    std::map<std::string, TradingPairPrice> cache; // maps symbol to price
    std::vector<Trade> trades;
    std::vector<Trade> profitableTrades;

    std::map<std::set<std::string>, TradingPair> tradingPairs;
    std::map<std::string, TradingPairPrice> tradingPairPrices;

    std::map<std::string, std::set<std::string>> symbolToTickers;

    BinanceClient();
    ~BinanceClient();

    // Delete Copy Constructor, Move Constructor, Copy Assignment Operator, Move
    // Assignment Operator Rule of 5: forced to define these after implementing
    // custom destructor
    BinanceClient(const BinanceClient&) = delete;
    BinanceClient(BinanceClient&&) = delete;
    auto operator=(const BinanceClient&) -> BinanceClient& = delete;
    auto operator=(BinanceClient&&) -> BinanceClient& = delete;

    auto buildTickersSymbolsTickersToSymbolsConnections() -> void;
    auto buildCycles() -> void;
    auto buildTrades() -> void;
    auto addPriceToTradingPairPrices(const std::string& name) -> void;
};
