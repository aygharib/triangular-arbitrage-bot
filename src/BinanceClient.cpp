#include "BinanceClient.h"

#include <fmt/core.h>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

BinanceClient::BinanceClient() {
    // Set up the program environment for libcurl
    // Must be called before any other libcurl function calls occur
    curl_global_init(CURL_GLOBAL_DEFAULT);

    buildTickersSymbolsTickersToSymbolsConnections();
    buildCycles();
    buildTrades();
}

BinanceClient::~BinanceClient() {
    curl_global_cleanup();
}

auto curlCallbackFunction(void* contents, size_t size, size_t nmemb, std::string* s) -> size_t {
    auto newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
    }
    catch(std::bad_alloc &e) {
        //handle memory problem
        return 0;
    }

    return newLength;
}

auto BinanceClient::buildTickersSymbolsTickersToSymbolsConnections() -> void {
    // Start a libcurl easy session
    auto* curl = curl_easy_init();

    std::string s;
    if (curl != nullptr) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.binance.com/api/v3/exchangeInfo");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); //only for https
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); //only for https
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlCallbackFunction);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); //remove this to disable verbose output

        // Perform the request, res will get the return code
        auto res = curl_easy_perform(curl);

        // Check for errors
        if(res != CURLE_OK) {
            fmt::print(stderr, "curl_easy_perform() failed: {}\n", curl_easy_strerror(res));

            // Clean up libcurl easy session
            curl_easy_cleanup(curl);
        }
    }

    auto parsedResponse = json::parse(s);
    auto symbolsResponse = parsedResponse["symbols"];

    for (const auto& obj : symbolsResponse) {
        auto symbol = obj["symbol"];
        auto baseAsset = obj["baseAsset"];
        auto quoteAsset = obj["quoteAsset"];
        auto status = obj["status"];

        if (status != "TRADING") {
            continue;
        }

        TradingPair tradingPair{symbol, baseAsset, quoteAsset};
        tradingPairs[{baseAsset, quoteAsset}] = tradingPair;

        symbolToTickers[symbol] = {baseAsset, quoteAsset};

        tickers.insert(baseAsset);
        tickers.insert(quoteAsset);

        symbols.insert(symbol);

        connections[baseAsset].push_back(quoteAsset);
        connections[quoteAsset].push_back(baseAsset);
    }
}

auto BinanceClient::buildCycles() -> void {
    std::map<std::string, std::vector<std::string>>::iterator it;

    for (it = connections.begin(); it != connections.end(); it++) {
        auto root = it->first;
        auto children = it->second;

        for (const auto& child : children) {
            const auto& v = blacklist[root];
            auto a = v.begin();
            auto b = v.end();

            if (std::find(a, b, child) != v.end()) {
                continue;
            }

            blacklist[child].push_back(root);

            auto leaves = connections[child];
            
            for (const auto& leaf : leaves) {
                const auto& v2 = blacklist[child];
                auto a2 = v2.begin();
                auto b2 = v2.end();

                if (std::find(a2, b2, leaf) != v2.end()) {
                    continue;
                }

                // if children contains leaf then its a cycle
                if (std::count(children.begin(), children.end(), leaf)) {
                    // Cycle cycle{root, child, leaf};
                    std::vector<std::string> cycle{root, child, leaf};
                    cycles.push_back(cycle);
                }
            }
        }
    }
}

auto BinanceClient::addPriceToTradingPairPrices(std::string name) -> void {
    // Start a libcurl easy session
    auto* curl = curl_easy_init();

    std::string s;
    if (curl != nullptr) {
        std::string temp;
        temp.append("https://api.binance.com/api/v3/depth?symbol=");
        temp.append(name);

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
        curl_easy_setopt(curl, CURLOPT_URL, temp.c_str());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); //only for https
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); //only for https
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlCallbackFunction);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); //remove this to disable verbose output

        // Perform the request, res will get the return code
        auto res = curl_easy_perform(curl);

        // Check for errors
        if(res != CURLE_OK) {
            fmt::print(stderr, "curl_easy_perform() failed: {}\n", curl_easy_strerror(res));

            // Clean up libcurl easy session
            curl_easy_cleanup(curl);
        }
    }

    auto j = json::parse(s);
    auto bids = j["bids"];
    auto asks = j["asks"];
    
    auto topBidPrice = std::stod(bids[0][0].get<nlohmann::json::string_t>());
    auto topBidQuantity = std::stod(bids[0][1].get<nlohmann::json::string_t>());
    auto topAskPrice = std::stod(asks[0][0].get<nlohmann::json::string_t>());
    auto topAskQuantity = std::stod(asks[0][1].get<nlohmann::json::string_t>());

    auto tradingPair = tradingPairs[symbolToTickers[name]];

    TradingPairPrice tradingPairPrice{topBidPrice, topAskPrice};
    tradingPairPrices[tradingPair.name] = tradingPairPrice;
}

auto BinanceClient::buildTrades() -> void {
    for (const auto& cycle : cycles) {
        double profit{0.};

        for (int i = 0; i < 3; i++) {
            auto ticker1 = cycle.at(i); // ETH
            auto ticker2 = cycle.at((i + 1) % 3); // BTC

            auto tradingPair = tradingPairs[{ticker1, ticker2}];

            // If tradingPairPrices doesn't contain this price, go calculate it and store it
            if (!tradingPairPrices.contains(tradingPair.name)) {
                // add to tradingPairPrices from a request
                addPriceToTradingPairPrices(tradingPair.name);
            }

            auto tradingPairPrice = tradingPairPrices[tradingPair.name];

            if (ticker1 == tradingPair.baseAsset) {
                profit += tradingPairPrice.askPrice;
            }
            else if (ticker1 == tradingPair.quoteAsset) {
                profit += tradingPairPrice.bidPrice;
            }
        }

        Trade trade{cycle, profit};
        trades.push_back(trade);
        break;
    }

    // clear tradingPairPrices before next iteration
    tradingPairPrices.clear();
}