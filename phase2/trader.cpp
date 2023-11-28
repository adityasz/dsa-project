#include <iostream>
#include <atomic>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <fstream>

#include "stl/unordered_map"
#include "stl/queue"
#include "stl/algorithm"

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


extern std::atomic<int> commonTimer;
extern std::mutex       printMutex;  // Declare the mutex for printing

struct order {
	std::string trader_name;

	int arrival;
	int expiry;
	int price;
	int quantity;
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

// class pqueue {
// public:
// 	std::vector<std::string> elements;
// 	size_t front;
// 	size_t rear;
// 	size_t capacity = 5;

// 	pqueue(size_t size) : elements(size), front(0),
// 	                      rear(0), capacity(size) {}


// 	void enqueue(std::string value)
// 	{
// 		if ((rear + 1) % capacity == front)
// 			dequeue();
// 		elements[rear] = value;
// 		rear = (rear + 1) % capacity;
// 	}

// 	void dequeue()
// 	{
// 		if (front == rear)
// 			return;
// 		front = (front + 1) % capacity;
// 	}

// };


int reader(int time)
{
	int t = commonTimer.load();
	// std::cout << t << " 22B0056_22B0636 SELL AMD $1 #32 3" << std::endl;
	// std::vector<std::vector<int>> bruh;
	// std::this_thread::sleep_for(std::chrono::milliseconds(500));

	// pqueue lit(5);
	std::vector<std::string> lines;
	std::ifstream inputFile ("output.txt");
	std::string line;

	// std::unordered_map<std::string, std::priority_queue<int, std::vector<int>, std::greater<int>>> min_heap;
	std::unordered_map<std::string, std::priority_queue<order, std::vector<order>, compare_buy>> buy_heaps;
	std::unordered_map<std::string, std::priority_queue<order, std::vector<order>, compare_sell>> sell_heaps;
	std::unordered_map<std::string, std::pair<std::priority_queue<int>, std::priority_queue<int>>> median_heaps;

	while (std::getline(inputFile, line)) {
		if (line.compare("TL") == 0)
			continue;

		if (line.compare("!@") == 0 ||
                    line.compare("Successfully Initiated!") == 0) {
			std::cout << "Hello ye aa gaya\n";
			break;
		}
		lines.push_back(line);

		__get_time(start);
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
					DEBUG_MSG(curr.trader_name << " purchased "
					          << curr.quantity << " share of "
					          << curr_stock << " from "
					          << heap.top().trader_name << " for $"
					          << heap.top().price << "/share\n");
					auto best = heap.top();
					heap.pop();
					best.quantity -= curr.quantity;
					heap.push(best);
					alive = false;
					break;
				}
				if (heap.top().quantity == curr.quantity) {
					DEBUG_MSG(curr.trader_name << " purchased "
					          << curr.quantity << " share of "
					          << curr_stock << " from "
					          << heap.top().trader_name << " for $"
					          << heap.top().price << "/share\n");
					heap.pop();
					alive = false;
					break;
				}
				if (heap.top().quantity < curr.quantity) {
					DEBUG_MSG(curr.trader_name << " purchased "
					          << heap.top().quantity << " share of "
					          << curr_stock << " from "
					          << heap.top().trader_name << " for $"
					          << heap.top().price << "/share\n");
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
					DEBUG_MSG(heap.top().trader_name << " purchased "
					          << curr.quantity << " share of "
					          << curr_stock << " from "
					          << curr.trader_name << " for $"
					          << heap.top().price << "/share\n");
					auto best = heap.top();
					heap.pop();
					best.quantity -= curr.quantity;
					heap.push(best);
					alive = false;
					break;
				}
				if (heap.top().quantity == curr.quantity) {
					DEBUG_MSG(heap.top().trader_name << " purchased "
					          << heap.top().quantity << " share of "
					          << curr_stock << " from "
					          << curr.trader_name << " for $"
					          << heap.top().price << "/share\n");
					heap.pop();
					alive = false;
					break;
				}
				if (heap.top().quantity < curr.quantity) {
					DEBUG_MSG(heap.top().trader_name << " purchased "
					          << heap.top().quantity << " share of "
					          << curr_stock << " from "
					          << curr.trader_name << " for $"
					          << heap.top().price << "/share\n");
					curr.quantity -= heap.top().quantity;
					heap.pop();
				}
			}
			if (alive)
				sell_heaps[curr_stock].push(curr);
		}
		__get_time(end);
		__duration(diff, start, end);
		DEBUG_MSG(diff << " s\n");

		{
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			t = commonTimer.load();
			std::lock_guard<std::mutex> lock(printMutex);
			std::cout << t << " 22B0056_22B0636 SELL AMD $50 #1000 3" << std::endl;
		}
	}

	// {
	// 	t = commonTimer.load();
	// 	std::lock_guard<std::mutex> lock(printMutex);
	// 	std::cout << t << " 22B0056_22B0636 SELL AMD $1 #32 3\r\n";
	// }
	return 1;
}

int trader(std::string *message)
{
	return 1;
}

void* userThread(void *arg)
{
	int thread_id = *(int *) arg;
	while (true) {
		int currentTime;

		{
			currentTime = commonTimer.load();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(500)); 
		int end = reader(currentTime);
		if (end) break;
	}
	return nullptr;
}

void* userTrader(void *arg)
{
	return nullptr;
}
