#include "market.h"
#include "stl/unordered_map"
#include "stl/climits"
#include "stl/queue"

#ifdef DEBUG
#include <chrono>
#define DEBUG_MSG(x)        std::cerr << x
#define __get_time(x)       auto x = std::chrono::steady_clock::now()
#define __duration(t, x, y) auto t = std::chrono::duration<double, std::milli> (y - x).count()
#define __print_vector(v)                                                      \
        do {                                                                   \
        	int __idx__ = 0;                                               \
        	for (auto x : v) {                                             \
        		if (__idx__++ > 8) {                                   \
        			DEBUG_MSG("...\n");                            \
        			break;                                         \
        		}                                                      \
        		DEBUG_MSG(x << ' ');                                   \
        	}                                                              \
        } while (0)
#else
#define DEBUG_MSG(x)         do {} while (0)
#define __get_time(x)        do {} while (0)
#define __duration(t, x, y)  do {} while (0)
#define __print_vector(v)    do {} while (0)
#endif

struct order {
	std::string trader;
	std::string stock;
	std::string type;

	int arrival;
	int duration;
	int expiry;
	int price;
	int quantity;
};

struct compare_expiry {
	bool operator()(const order &a, const order &b) const {
		if (a.expiry < b.expiry)
			return true;
		if (a.expiry > b.expiry)
			return false;
		if (a.arrival < b.arrival)
			return true;
		if (a.arrival > b.arrival)
			return false;
		if (a.trader < b.trader)
			return true;
		return false;
		// TODO (linked to second todo): agar trader bhi equal ho gaya
		// to gadbad ho gayi
	}
};

struct compare_buy {
	bool operator()(const order &a, const order &b) const {
		if (a.price > b.price)
			return true;
		if (a.price < b.price)
			return false;
		if (a.arrival < b.arrival)
			return true;
		if (a.arrival > b.arrival)
			return false;
		if (a.trader < b.trader)
			return true;
		return false;
	}
};

struct compare_sell {
	bool operator()(const order &a, const order &b) const {
		if (a.price < b.price)
			return true;
		if (a.price > b.price)
			return false;
		if (a.arrival < b.arrival)
			return true;
		if (a.arrival > b.arrival)
			return false;
		if (a.trader < b.trader)
			return true;
		return false;
	}
};

market::market(int argc, char *argv[])
{
	std::ifstream inputFile("output.txt");
	std::string   line;

	while (std::getline(inputFile,line)) {
		if (line.compare("TL") == 0)
			continue;
		if (line.compare("!@") == 0 ||
		    line.compare("Successfully Initiated!") == 0)
			break;
		lines.push_back(line);
	}
}

void market::start()
{
	// TODO: Update code if two (or more) lines can be the same

	// // (stock name, (expiry_heap, buy_heap, sell_heap))
	// std::unordered_map<std::string,
                           // std::tuple<std::priority_queue<std::reference_wrapper<order>, std::vector<std::reference_wrapper<order>>, compare_buy>,
                                      // std::priority_queue<std::reference_wrapper<order>, std::vector<std::reference_wrapper<order>>, compare_sell>,
                                      // std::priority_queue<std::reference_wrapper<order>, std::vector<std::reference_wrapper<order>>, compare_expiry>>>

			   // heaps;

	std::unordered_map<std::string, std::priority_queue<order, std::vector<order>, compare_buy>> buy_heaps;
	std::unordered_map<std::string, std::priority_queue<order, std::vector<order>, compare_sell>> sell_heaps;

	for (auto line : lines) {
		DEBUG_MSG("──────────────────────────────────────────────────────────────────────\n");
		order curr;
		int amount;
		int num_trades;
		int num_shares;

		std::string arrival_str;
		std::string price_str;
		std::string qty_str;
		std::string duration_str;

		int  idx = -1;
		// Get arrival time
		while (line[++idx] != ' ')
			arrival_str += line[idx];
		curr.arrival = std::stoi(arrival_str);

		// Get trader name
		while (line[++idx] != ' ')
			curr.trader += line[idx];

		// Get type
		while (line[++idx] != ' ')
			curr.type += line[idx];

		// Get stock
		while (line[++idx] != '$')
			curr.stock += line[idx];
		curr.stock.pop_back();
		
		// Get linear combination
		if (curr.stock.find(' ') != std::string::npos) {
			curr.stock.push_back(' ');
			std::vector<std::string> stocks;
			int i = -1;
			do {
				std::string stock_name;
				std::string qty_str;

				while (curr.stock[++i] != ' ')
					stock_name += curr.stock[i];
				if (curr.stock[i + 1] <= '9') {
					while (curr.stock[++i] != ' ') 
						qty_str += curr.stock[i];
				}

				stocks.push_back(stock_name + qty_str);
			} while (i < curr.stock.size() - 1);
			curr.stock = "";
			std::sort(stocks.begin(), stocks.end());
			for (auto stock : stocks)
				curr.stock += stock;
		}

		while (line[++idx] != ' ')
			price_str += line[idx];
		curr.price = std::stoi(price_str);

		// Get quantity
		idx++;	// ignore '#'
		while (line[++idx] != ' ')
			qty_str += line[idx];
		curr.quantity = std::stoi(qty_str);

		// Get duration
		idx++;
		while (line[idx] != '\r' && line[idx] != '\n')
			duration_str += line[idx++];
		curr.duration = std::stoi(duration_str);
		if (curr.duration == -1) {
			curr.duration = INT_MAX;
			curr.expiry = INT_MAX;
		} else {
			curr.expiry = curr.arrival + curr.duration;
		}
		
		DEBUG_MSG(line << '\n');
		DEBUG_MSG("Arrival time: " << curr.arrival);
		DEBUG_MSG("\tDuration: " << curr.duration);
		DEBUG_MSG("\tExpiry time: " << curr.expiry << '\n');
		DEBUG_MSG("Trader: " << curr.trader);
		DEBUG_MSG("\tStock: " << curr.stock << '\n');
		DEBUG_MSG("Amount: " << curr.price);
		DEBUG_MSG("\tQuantity: " << curr.quantity << '\n');

		// for (auto it : heaps) {
		// 	while (std::get<0>(it.second).top().get().arrival < curr.arrival) {
		// 		std::get<0>(it.second).top().get().alive = false;
		// 		std::get<0>(it.second).pop();
		// 	}
		// }

		// switch (curr.type[0]) {
		// case 'B':
		// 	while (std::get<2>(heaps[curr.stock]).top().get().price <= curr.price) {
		// 		if (std::get<2>(heaps[curr.stock]).top().get().quantity > curr.quantity) {
		// 			std::get<2>(heaps[curr.stock]).top().get().quantity -= curr.quantity;
		// 			break;
		// 		}
		// 	}
		// 	break;
		// case 'S':
		// 	break;
		// }

		if (curr.type[0] == 'B') {
			auto &heap = sell_heaps[curr.stock];
			if (heap.empty()) {
				buy_heaps[curr.stock].push(curr);
				continue;
			}
			int alive = true;
			while (heap.top().price <= curr.price) {
				if (heap.top().expiry < curr.arrival)
					heap.pop();
				if (heap.empty()) {
					buy_heaps[curr.stock].push(curr);
					break;
				}
				if (heap.top().quantity > curr.quantity) {
					DEBUG_MSG(curr.trader << " purchased "
					          << curr.quantity << " share of "
					          << curr.stock << " from "
					          << heap.top().trader << " for $"
					          << heap.top().price << "/share\n");
					std::cout << curr.trader << " purchased "
					          << curr.quantity << " share of "
					          << curr.stock << " from "
					          << heap.top().trader << " for $"
					          << heap.top().price << "/share\n";
					auto best = heap.top();
					heap.pop();
					best.quantity -= curr.quantity;
					heap.push(best);
					alive = false;
					break;
				}
				if (heap.top().quantity == curr.quantity) {
					DEBUG_MSG(curr.trader << " purchased "
					          << curr.quantity << " share of "
					          << curr.stock << " from "
					          << heap.top().trader << " for $"
					          << heap.top().price << "/share\n");
					std::cout << curr.trader << " purchased "
					          << curr.quantity << " share of "
					          << curr.stock << " from "
					          << heap.top().trader << " for $"
					          << heap.top().price << "/share\n";
					heap.pop();
					alive = false;
					break;
				}
				if (heap.top().quantity < curr.quantity) {
					DEBUG_MSG(curr.trader << " purchased "
					          << heap.top().quantity << " share of "
					          << curr.stock << " from "
					          << heap.top().trader << " for $"
					          << heap.top().price << "/share\n");
					std::cout << curr.trader << " purchased "
					          << heap.top().quantity << " share of "
					          << curr.stock << " from "
					          << heap.top().trader << " for $"
					          << heap.top().price << "/share\n";
					heap.pop();
					curr.quantity -= heap.top().quantity;
				}
				if (heap.empty()) {
					buy_heaps[curr.stock].push(curr);
					break;
				}
			}
			if (alive)
				buy_heaps[curr.stock].push(curr);
		} else {
			auto &heap = buy_heaps[curr.stock];
			if (heap.empty()) {
				sell_heaps[curr.stock].push(curr);
				continue;
			}
			int alive = true;
			while (heap.top().price >= curr.price) {
				if (heap.top().expiry < curr.arrival)
					heap.pop();
				if (heap.empty()) {
					sell_heaps[curr.stock].push(curr);
					break;
				}
				if (heap.top().quantity > curr.quantity) {
					DEBUG_MSG(heap.top().trader << " purchased "
					          << curr.quantity << " share of "
					          << curr.stock << " from "
					          << curr.trader << " for $"
					          << heap.top().price << "/share\n");
					std::cout << heap.top().trader << " purchased "
					          << curr.quantity << " share of "
					          << curr.stock << " from "
					          << curr.trader << " for $"
					          << heap.top().price << "/share\n";
					auto best = heap.top();
					heap.pop();
					best.quantity -= curr.quantity;
					heap.push(best);
					alive = false;
					break;
				}
				if (heap.top().quantity == curr.quantity) {
					DEBUG_MSG(heap.top().trader << " purchased "
					          << heap.top().quantity << " share of "
					          << curr.stock << " from "
					          << curr.trader << " for $"
					          << heap.top().price << "/share\n");
					std::cout << heap.top().trader << " purchased "
					          << heap.top().quantity << " share of "
					          << curr.stock << " from "
					          << curr.trader << " for $"
					          << heap.top().price << "/share\n";
					heap.pop();
					alive = false;
					break;
				}
				if (heap.top().quantity < curr.quantity) {
					DEBUG_MSG(heap.top().trader << " purchased "
					          << heap.top().quantity << " share of "
					          << curr.stock << " from "
					          << curr.trader << " for $"
					          << heap.top().price << "/share\n");
					std::cout << heap.top().trader << " purchased "
					          << heap.top().quantity << " share of "
					          << curr.stock << " from "
					          << curr.trader << " for $"
					          << heap.top().price << "/share\n";
					heap.pop();
					curr.quantity -= heap.top().quantity;
				}
				if (heap.empty()) {
					sell_heaps[curr.stock].push(curr);
					continue;
				}
			}
			if (alive)
				sell_heaps[curr.stock].push(curr);
		}
	}

	DEBUG_MSG("──────────────────────────────────────────────────────────────────────\n");
}
