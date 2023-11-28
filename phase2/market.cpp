#include "market.h"
#include "stl/unordered_map"
#include "stl/climits"
#include "stl/queue"
#include "stl/unordered_set"

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
	std::string trader_name;

	int arrival;
	int expiry;
	int price;
	int quantity;
};

struct stats {
	int num_sold;
	int num_bought;
	int net_transfer;

	stats() : num_sold(0), num_bought(0), net_transfer(0) {}
};

struct compare_buy {
	bool operator()(const order &a, const order &b) const
	{
		if (a.price < b.price)
			return true;
		if (a.price > b.price)
			return false;
		if (a.arrival > b.arrival)
			return true;
		if (a.arrival < b.arrival)
			return false;
		if (a.trader_name > b.trader_name)
			return true;
		return false;
	}
};

struct compare_sell {
	bool operator()(const order &a, const order &b) const
	{
		if (a.price > b.price)
			return true;
		if (a.price < b.price)
			return false;
		if (a.arrival > b.arrival)
			return true;
		if (a.arrival < b.arrival)
			return false;
		if (a.trader_name > b.trader_name)
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

	std::unordered_map<std::string, std::priority_queue<order, std::vector<order>, compare_buy>> buy_heaps;
	std::unordered_map<std::string, std::priority_queue<order, std::vector<order>, compare_sell>> sell_heaps;

	int amount = 0;
	int num_trades = 0;
	int num_shares = 0;
	std::unordered_map<std::string, stats> statistics;

	for (auto line : lines) {
		DEBUG_MSG("──────────────────────────────────────────────────────────────────────\n");
		DEBUG_MSG(line << '\n');

		order curr;

		std::string curr_stock;
		std::string arrival_str;
		std::string price_str;
		std::string qty_str;
		std::string duration_str;
		std::string curr_type;
		int         duration;

		int  idx = -1;
		// Get arrival time
		// while (!('9' - line[idx + 1] >= 0 && line[idx + 1] - '0' >= 0))
		// 	idx++; // Skip garbage
		while (line[++idx] != ' ')
			arrival_str += line[idx];
		curr.arrival = std::stoi(arrival_str);

		// Get trader_name name
		while (line[++idx] != ' ')
			curr.trader_name += line[idx];

		// Get type
		while (line[++idx] != ' ')
			curr_type += line[idx];

		// Get stock
		while (line[++idx] != '$')
			curr_stock += line[idx];
		curr_stock.pop_back();
		
		// Get linear combination
		if (curr_stock.find(' ') != std::string::npos) {
			curr_stock.push_back(' ');
			std::vector<std::string> stocks;
			int i = -1;
			do {
				std::string stock_name;
				std::string qty_str;

				while (curr_stock[++i] != ' ')
					stock_name += curr_stock[i];
				if (curr_stock[i + 1] <= '9') {
					while (curr_stock[++i] != ' ') 
						qty_str += curr_stock[i];
				}

				stocks.push_back(stock_name + qty_str);
			} while (i < curr_stock.size() - 1);
			curr_stock = "";
			std::sort(stocks.begin(), stocks.end());
			for (auto stock : stocks)
				curr_stock += stock;
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
		while (idx < line.size() &&
		       (('9' - line[idx] >= 0 && line[idx] - '0' >= 0) || line[idx] == '-'))
			duration_str += line[idx++];
		duration = std::stoi(duration_str);
		if (duration == -1) {
			curr.expiry = INT_MAX;
		} else {
			curr.expiry = curr.arrival + duration;
		}
		
		DEBUG_MSG("Arrival time: " << curr.arrival);
		DEBUG_MSG("\tDuration: " << duration);
		DEBUG_MSG("\tExpiry time: " << curr.expiry << '\n');
		DEBUG_MSG("Trader: " << curr.trader_name);
		DEBUG_MSG("\tStock: " << curr_stock << '\n');
		DEBUG_MSG("Amount: " << curr.price);
		DEBUG_MSG("\tQuantity: " << curr.quantity << '\n');

		if (curr_type[0] == 'B') {
			auto &heap = sell_heaps[curr_stock];
			int alive = true;
			while (!heap.empty() && heap.top().price <= curr.price) {
				if (heap.top().expiry < curr.arrival) {
					DEBUG_MSG("Popped from " << curr_stock << " sell heap: "
					       << heap.top().trader_name << " $"
					       << heap.top().price << " #"
					       << heap.top().quantity << "\n");
					DEBUG_MSG("Reason: expired\n");
					heap.pop();
					continue;
				}
				if (heap.top().quantity > curr.quantity) {
					amount += heap.top().price * curr.quantity;
					num_trades++;
					num_shares += curr.quantity;
					statistics[heap.top().trader_name].num_sold += curr.quantity;
					statistics[heap.top().trader_name].net_transfer += heap.top().price * curr.quantity;
					statistics[curr.trader_name].num_bought += curr.quantity;
					statistics[curr.trader_name].net_transfer -= heap.top().price * curr.quantity;
					DEBUG_MSG(curr.trader_name << " purchased "
					          << curr.quantity << " share of "
					          << curr_stock << " from "
					          << heap.top().trader_name << " for $"
					          << heap.top().price << "/share\n");
					std::cout << curr.trader_name << " purchased "
					          << curr.quantity << " share of "
					          << curr_stock << " from "
					          << heap.top().trader_name << " for $"
					          << heap.top().price << "/share\n";
					auto best = heap.top();
					heap.pop();
					best.quantity -= curr.quantity;
					heap.push(best);
					alive = false;
					break;
				}
				if (heap.top().quantity == curr.quantity) {
					amount += heap.top().price * curr.quantity;
					num_trades++;
					num_shares += curr.quantity;
					statistics[heap.top().trader_name].num_sold += curr.quantity;
					statistics[heap.top().trader_name].net_transfer += heap.top().price * curr.quantity;
					statistics[curr.trader_name].num_bought += curr.quantity;
					statistics[curr.trader_name].net_transfer -= heap.top().price * curr.quantity;
					DEBUG_MSG(curr.trader_name << " purchased "
					          << curr.quantity << " share of "
					          << curr_stock << " from "
					          << heap.top().trader_name << " for $"
					          << heap.top().price << "/share\n");
					std::cout << curr.trader_name << " purchased "
					          << curr.quantity << " share of "
					          << curr_stock << " from "
					          << heap.top().trader_name << " for $"
					          << heap.top().price << "/share\n";
					heap.pop();
					alive = false;
					break;
				}
				if (heap.top().quantity < curr.quantity) {
					amount += heap.top().price * heap.top().quantity;
					num_trades++;
					num_shares += heap.top().quantity;
					statistics[heap.top().trader_name].num_sold += heap.top().quantity;
					statistics[heap.top().trader_name].net_transfer += heap.top().price * heap.top().quantity;
					statistics[curr.trader_name].num_bought += heap.top().quantity;
					statistics[curr.trader_name].net_transfer -= heap.top().price * heap.top().quantity;
					DEBUG_MSG(curr.trader_name << " purchased "
					          << heap.top().quantity << " share of "
					          << curr_stock << " from "
					          << heap.top().trader_name << " for $"
					          << heap.top().price << "/share\n");
					std::cout << curr.trader_name << " purchased "
					          << heap.top().quantity << " share of "
					          << curr_stock << " from "
					          << heap.top().trader_name << " for $"
					          << heap.top().price << "/share\n";
					curr.quantity -= heap.top().quantity;
					heap.pop();
				}
			}
			if (alive)
				buy_heaps[curr_stock].push(curr);
		} else {
			auto &heap = buy_heaps[curr_stock];
			int alive = true;
			while (!heap.empty() && heap.top().price >= curr.price) {
				if (heap.top().expiry < curr.arrival) {
					DEBUG_MSG("Popped from " << curr_stock << " buy heap: "
					       << heap.top().trader_name << " $"
					       << heap.top().price << " #"
					       << heap.top().quantity << "\n");
					DEBUG_MSG("Reason: expired\n");
					heap.pop();
					continue;
				}
				if (heap.top().quantity > curr.quantity) {
					amount += heap.top().price * curr.quantity;
					num_trades++;
					num_shares += curr.quantity;
					statistics[heap.top().trader_name].num_bought += curr.quantity;
					statistics[heap.top().trader_name].net_transfer -= heap.top().price * curr.quantity;
					statistics[curr.trader_name].num_sold += curr.quantity;
					statistics[curr.trader_name].net_transfer += heap.top().price * curr.quantity;
					DEBUG_MSG(heap.top().trader_name << " purchased "
					          << curr.quantity << " share of "
					          << curr_stock << " from "
					          << curr.trader_name << " for $"
					          << heap.top().price << "/share\n");
					std::cout << heap.top().trader_name << " purchased "
					          << curr.quantity << " share of "
					          << curr_stock << " from "
					          << curr.trader_name << " for $"
					          << heap.top().price << "/share\n";
					auto best = heap.top();
					heap.pop();
					best.quantity -= curr.quantity;
					heap.push(best);
					alive = false;
					break;
				}
				if (heap.top().quantity == curr.quantity) {
					amount += heap.top().price * heap.top().quantity;
					num_trades++;
					num_shares += heap.top().quantity;
					statistics[heap.top().trader_name].num_bought += heap.top().quantity;
					statistics[heap.top().trader_name].net_transfer -= heap.top().price * heap.top().quantity;
					statistics[curr.trader_name].num_sold += heap.top().quantity;
					statistics[curr.trader_name].net_transfer += heap.top().price * heap.top().quantity;
					DEBUG_MSG(heap.top().trader_name << " purchased "
					          << heap.top().quantity << " share of "
					          << curr_stock << " from "
					          << curr.trader_name << " for $"
					          << heap.top().price << "/share\n");
					std::cout << heap.top().trader_name << " purchased "
					          << heap.top().quantity << " share of "
					          << curr_stock << " from "
					          << curr.trader_name << " for $"
					          << heap.top().price << "/share\n";
					heap.pop();
					alive = false;
					break;
				}
				if (heap.top().quantity < curr.quantity) {
					amount += heap.top().price * heap.top().quantity;
					num_trades++;
					num_shares += heap.top().quantity;
					statistics[heap.top().trader_name].num_bought += heap.top().quantity;
					statistics[heap.top().trader_name].net_transfer -= heap.top().price * heap.top().quantity;
					statistics[curr.trader_name].num_sold += heap.top().quantity;
					statistics[curr.trader_name].net_transfer += heap.top().price * heap.top().quantity;
					DEBUG_MSG(heap.top().trader_name << " purchased "
					          << heap.top().quantity << " share of "
					          << curr_stock << " from "
					          << curr.trader_name << " for $"
					          << heap.top().price << "/share\n");
					std::cout << heap.top().trader_name << " purchased "
					          << heap.top().quantity << " share of "
					          << curr_stock << " from "
					          << curr.trader_name << " for $"
					          << heap.top().price << "/share\n";
					curr.quantity -= heap.top().quantity;
					heap.pop();
				}
			}
			if (alive)
				sell_heaps[curr_stock].push(curr);
		}
	}

	std::cout << "---End of day---\n";
	std::cout << "Total Amount of Money Transferred: $" << amount << '\n';
	std::cout << "Number of Completed Trades: " << num_trades << '\n';
	std::cout << "Number of Shares Traded: " << num_shares << '\n';
	for (auto it : statistics) {
		std::cout << it.first << " bought " << it.second.num_bought
		          << " and sold " << it.second.num_sold
			  << " for a net transfer of $" << it.second.net_transfer
		          << '\n';
	}
	DEBUG_MSG("──────────────────────────────────────────────────────────────────────\n");
}
