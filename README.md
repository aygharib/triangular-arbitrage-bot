# Trading Bot

Monitors all currencies on the Binance crypto-currency exchange in search for triangular arbitrage opportunities. The problem this app aims to solve is finding all profitable arbitrage opportunities in as little time as possible.

<details>
<summary>Sections</summary>
<p>

- [Glossary](#glossary)
- [Potential approaches](#potential-approaches)
  - [Graph traversal](#graph-traversal)
    - [Floyd-Warshall algorithm](#floyd-warshall-algorithm)
    - [Johnson's algorithm](#johnsons-algorithm)
  - [Iterative traversal](#iterative-traversal)
    - [Iterative Approach 1](#iterative-approach-1)
    - [Iterative Approach 2](#iterative-approach-2)
- [Links](#links)

</p>
</details>

## Glossary
- **Arbitrage**: Buying and selling of assets over different markets in order to take advantage of differing prices on the same asset.
- **Ticker symbol**: The trading 'symbol' or shortened name (typically in capital letters) that refer to a coin on a trading platform. For example: `BTC`
- **Trading symbol/Trading pair**: The symbol that represents a possible trade between two ticker symbols. Is typically represented by two ticker symbols concatentated. For example: `BTCETH` represents the ability to sell BTC to obtain ETH, or vice-versa.
- **Base asset and Quote asset**: On a trading exchange assets are traded in pairs, or in other words, one asset (Base asset) is traded against the other (Quote asset). The spot rate (spot price) reflects how much of the Quote asset is required to buy 1 unit of the Base asset. For example, if our desired trading pair is BTC/USDT, and the spot rate is 30,000 USDT, we will be trading BTC, which is the Base asset, against USDT, which is the Quote asset. The spot rate of 30,000 USDT means that we have to sell 30,000 USDT to obtain 1 BTC.
- **Ask Price**: The lowest price a seller is willing to accept on their sell order when trading an asset on an exchange.
- **Bid Price**: In the context of financial markets, it is the value buyers offer for an asset, such as a commodity, security, or cryptocurrency.
- **Order Book**: An electronic list of outstanding buy and sell orders for a specific asset on an exchange or marketplace.

## Potential approaches
### Graph traversal
For solutions to the stated problem, a directed edge-weighted graph can be used. A graph `G`, with vertices `V`, that represent a currency with edges `E`, that represent the conversion rate between currencies. By setting up the graph in a way that models its real-world application, it becomes a shortest path graph problem to find which series of transactions (or cycles) are profitable. To find the profitable cycles within this input graph, the Floyd-Warshall or Johnson's algorithm can be employed. Both Floyd-Warshall algorithm and Johnson's algorithm are very similar in the fact that they find the shortest paths in a graph.

Time complexity to build graph vertices: `O(V)` \
Time complexity to build graph edges: `O(V * (V - 1))` (see: https://stackoverflow.com/questions/5058406/what-is-the-maximum-number-of-edges-in-a-directed-graph-with-n-nodes)

### Floyd-Warshall algorithm
A shortest-path graph algorithm to compute the shortest path in a graph. Unlike Djikstra's algorithm, the Floyd-Warshall algorithm is not a single-sourced meaning that is computes the shortest distance between all vertices of the graph rather than computing distances from only one source vertex.

Time complexity to calculate profitability: `O(V^3)`

### Johnson's algorithm
Uses Bellmand-Ford algorithm to reweigh the input graph to elminate negative edges, thereby removing any negative cycles as a result of this transformation. Performs the *all pairs shortest path problem* on the input graph and outputs the shortest path between every pair of vertices in the graph.

Lists all elementary (simple) cycles of an input weighted directed graph.

Time complexity to calculate profitability: `O(V^2 * log(V) + V*E)`

### Floyd-Warshall vs Johnson's
Both algorithms solve the stated problem, but there is a difference between the time complexities for these solutions. Although the Floyd-Warshall algorithm runs in cubic time compared to Johnson's algorithms squared time, the latter's time complexity depends on the number of edges in the graph. Johnson's algorithm is preferred in cases where a graph is sparse; cases in which the number of edges in a graph is much less than the possible number of edges. The graph being built to solve the stated problem has the absolute maximum number of possible edges of (n^2 - n)/2 edges to link every currency with all other currencies based on the conversion rate as a weighted edge. The Floyd-Warshall algorithm is preferred as the number of vertices being used is relatively small and isn't affected by the number of edges being used.

### Iterative traversal

### Iterative Approach 1
Calculates cycles by iterating over an array of symbols and exchange rate, storing profitable cycles in an output array to be sorted from most-to-least profitable. The iterative approach will build an array `currencies` only once, keeping track of all currencies traded on the market, then build an array/hashmap `conversions` for each iteration to store all currency conversion rates between currencies. Then, using both `currencies` and `conversions`, the profitability is calculated by iterating on all elements of `currencies` using `conversions` to lookup conversion rates. Profitable cycles are stored in a final array `profitableCycles` to be sorted before being output to the user.

The big assumption with this approach, which makes it not completely viable, is that it assumes that there exists a viable trade opportunity between any two currencies. There exist currencies that are only able to convert between a few other currencies, which is vastly different to more widely adopted currencies like ETH and BTC that support many trade-pairs. Due to this fact, it makes more sense to approach this problem with a Graph rather than using an iterative approach.

#### Time complexity analysis of build array/map of conversions
For example, consider the following case, where V is the number of supported currencies.
```
V = 10
Currencies: a, b, c, d, e, f, g, h, i, j

a-b,a-c,a-d,a-e,a-f,a-g,a-h,a-i,a-j  <-- 9 edges
b-c,b-d,b-e,b-f,b-g,b-j,b-i,b-j      <-- 8 edges
c-d,c-e,c-f,c-g,c-h,c-i,c-j          <-- 7 edges 
d-e,d-f,d-g,d-h,d-i,d-j              <-- 6 edges
e-f,e-g,e-h,e-i,e-j                  <-- 5 edges
f-g,f-h,f-i,f-j                      <-- 4 edges
g-h,g-i,g-j                          <-- 3 edges
h-i,h-j                              <-- 2 edges
i-j                                  <-- 1 edges
```

9+8+7+6+5+4+3+2+1=45 \
(n^2-n)/2 = total number of edges

Complexity = `O(((V-1)^2 - (V-1)) / 2)`

Time complexity to build array of currencies: `O(V)` \
Time complexity to build array/map of conversions: `O(((V-1)^2 - (V-1))/ 2)` \
Time complexity to calculate profitability: `O(V^3)`

Overall time complexity: `O(V) + O(((V-1)^2 - (V-1))/ 2) + O(V^3)`

### Iterative Approach 2
**The following steps only occur once at the start of program execution to intialize data structures:**
1. Build a `std::set<std::string> tickers`, containing all the tickers supported on the exchange. Uses `/api/v3/exchangeInfo`.
2. Build a `std::set<std::string> symbols`, containing all the symbols supported on the exchange. Uses `/api/v3/exchangeInfo`.
3. Build a `std::map<std::set<std::string>, std::string> tickersToSymbols`, where the set contains the two tickers that map to the trading pair. Example: `tickersToSymbols["BTC", "ETH"]` returns `BTCETH`
3. Build a `std::map<std::string, std::vector<std::string>> connections`, where the key is a ticker and the value is a vector of all tickers the key ticker can trade to. Uses `/api/v3/exchangeInfo`.
4. Build a `std::vector<Cycle> cycles`, containing all possible trading cycles of length 3. Example of cycles: `Cycle{"BTC", "ETH", "BNB"}, Cycle{"BNB", "ETH", "BTC"}, Cycle{"ABC", "DEF", "GHI"}, Cycle{"GHI", "DEF", "ABC"}`. These there exist two cycles for every trianglular cycle, one in each trading direction. This duplication allows for much simpler calculation when calculating profitability.

**The following steps repeat every iteration to calculate profitable cycles:**
1. Build a `std::map<std::string, double> cache`, that stores the price of a trading pair if that trading pair. Subsequent calls to the same trading pair in the same iteration will use the cached value rather than sending another request. This cache is used when calculating cycle profitability. Uses `/api/v3/depth` for each trading pair. `cache` gets cleared at the end of every loop to get rid of outdated prices.
2. Build a `std::vector<Trade> trades`, building trades by looping through every `Cycle` in `cycles`. For every sequential pair, do a lookup in `tickersToSymbols` to get the symbol, and then check `cache` if that symbol exists in the cache. If it does then use the price stored in the cache to calculate the profitability. If it doesn't exist in the cache then do previous step.
3. Every `Trade` in `trades` with a profit >= 1 gets stored in `std::vector<Trade> profitableTrades`.

## Bottlenecks
There are a few bottlenecks to consider to reduce latency and increase performance of this bot.

### Request Latency
Issue: Requests to Binance server take on average ~150ms (+100-300ms) from Toronto.\
Resolution: Move closer to Binance servers to get order book price updates with less latency.

### Synchronous vs Asynchronous requests
1. Approach: Purely synchronous requests
  - When processing cycles iteratively, fetch symbol prices on a need-to-know basis synchronously.
    - Example: Cycle - BTC <-> ETH <-> BNB, `request1` fetches BTCETH price, `request2` fetches `ETHBNB` price, and `request3` fetches `BTCBNB` price.
  - Will give false positives for cycles since each price update will take a minimum of 150ms, resulting in cycles processed later to used outdated prices when calculating profitability
2. Approach: Asynchronous requests
  - Before processing cycle profitibilties, send out a blast of asynchronous requests for all symbols and only calculate cycle profitability once all requests have a response
  - Better than approach 1 as we can be somewhat confident that all the market data we receive were taken at roughly the same point in time
3. Approach: Purely asynchronous requests
  - Continually poll for the most up-to-date data
  - Too difficult to implement, and the benefits gained from implementing this approach are overshadowed by reducing request latency. It's better to move closer to the servers and get near instantaneous results that way.

## Links
- https://www.cs.tufts.edu/comp/150GA/homeworks/hw1/Johnson%2075.PDF
- https://brilliant.org/wiki/johnsons-algorithm/
- https://brilliant.org/wiki/floyd-warshall-algorithm/
- https://stackoverflow.com/questions/546655/finding-all-cycles-in-a-directed-graph
- https://stackoverflow.com/questions/5058406/what-is-the-maximum-number-of-edges-in-a-directed-graph-with-n-nodes
- https://math.stackexchange.com/questions/593318/factorial-but-with-addition