# Trading Bot

Monitors all currencies on the Binance crypto-currency exchange in search for triangular arbitrage opportunities. The problem this app aims to solve is finding all profitable arbitrage opportunities in as little time as possible.

<details>
<summary>Sections</summary>
<p>

- [Potential approaches](#potential-approaches)
  - [Graph traversal](#graph-traversal)
    - [Floyd-Warshall algorithm](#floyd-warshall-algorithm)
    - [Johnson's algorithm](#johnsons-algorithm)
  - [Iterative traversal](#iterative-traversal)
- [Links](#links)

</p>
</details>

## Potential approaches
### Graph traversal
For solutions to the stated problem, a directed edge-weighted graph can be used. A graph `G`, with vertices `V`, that represent a currency with edges `E`, that represent the conversion rate between currencies. By setting up the graph in a way that models its real-world application, it becomes a shortest path graph problem to find which series of transactions (or cycles) are profitable. To find the profitable cycles within this input graph, the Floyd-Warshall or Johnson's algorithm can be employed. Both Floyd-Warshall algorithm and Johnson's algorithm are very similar in the fact that they find the shortest paths in a graph.

Time complexity to build graph vertices: `O(V)` \
Time complexity to build graph edges: `O(V * (V - 1))` (see: https://stackoverflow.com/questions/5058406/what-is-the-maximum-number-of-edges-in-a-directed-graph-with-n-nodes)

### Floyd-Warshall algorithm
A shortest-path graph algorithm to compute the shortest path in a graph. Unlike Djikstra's algorithm, the Floyd-Warshall algorithm is not a single-sourced meaning that is computes the shortest distance between all vertices of the graph rather than computing distances from only one source vertex.

Time complexity to calculate profitability: O(V^3)

### Johnson's algorithm
Uses Bellmand-Ford algorithm to reweigh the input graph to elminate negative edges, thereby removing any negative cycles as a result of this transformation. Performs the *all pairs shortest path problem* on the input graph and outputs the shortest path between every pair of vertices in the graph.

Lists all elementary (simple) cycles of an input weighted directed graph.

Time complexity to calculate profitability: O(V^2 * log(V) + V*E)

### Floyd-Warshall vs Johnson's
Both algorithms solve the stated problem, but there is a difference between the time complexities for these solutions. Although the Floyd-Warshall algorithm runs in cubic time compared to Johnson's algorithms squared time, the latter's time complexity depends on the number of edges in the graph. Johnson's algorithm is preferred in cases where a graph is sparse; cases in which the number of edges in a graph is much less than the possible number of edges. The graph being built to solve the stated problem has the absolute maximum number of possible edges of (n^2 - n)/2 edges to link every currency with all other currencies based on the conversion rate as a weighted edge. The Floyd-Warshall algorithm is preferred as the number of vertices being used is relatively small and isn't affected by the number of edges being used.

### Iterative traversal
Calculates cycles by iterating over an array of symbols and exchange rate, storing profitable cycles in an output array to be sorted from most-to-least profitable. The iterative approach will build an array once storing all the currencies supported `currencies`, and build an array/hashmap for each iteration to store all currency conversion rates between currencies `conversions`. Then, using both `currencies` and `conversions`, the profitability is calculated by iterating on all elements of `currencies` using `conversions` to lookup conversion rates. Profitable cycles are stored in a final array `profitableCycles` to be sorted before being output to the user.

The big assumption with this approach, which makes it not completely viable, is that it assumes that there exists a viable trade opportunity between any two currencies. There exist currencies that are only able to convert between a few other currencies, which is vastly different to more widely adopted currencies like ETH and BTC that support many trade-pairs. Due to this fact, it makes more sense to approach this problem with a Graph rather than using an iterative approach.

#### Time complexity analysis of `build array/map of conversions
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

Complexity = O(((V-1)^2 - (V-1)) / 2)

Time complexity to build array of currencies: `O(V)` \
Time complexity to build array/map of conversions: `O(((V-1)^2 - (V-1))/ 2)` \
Time complexity to calculate profitability: `O(V^3)`

Overall time complexity: `O(V) + O(((V-1)^2 - (V-1))/ 2) + O(V^3)`

## Links
- https://www.cs.tufts.edu/comp/150GA/homeworks/hw1/Johnson%2075.PDF
- https://brilliant.org/wiki/johnsons-algorithm/
- https://brilliant.org/wiki/floyd-warshall-algorithm/
- https://stackoverflow.com/questions/546655/finding-all-cycles-in-a-directed-graph
- https://stackoverflow.com/questions/5058406/what-is-the-maximum-number-of-edges-in-a-directed-graph-with-n-nodes
- https://math.stackexchange.com/questions/593318/factorial-but-with-addition