#include "stl/algorithm"

struct order_q {
	std::unordered_map<std::string, long long> quantities;
	int         price;
	char        type;
	long long   quantity;
	std::string name;
};

bool arbitrage(int num_alive, std::vector<order_q> &alive_orders,
               std::vector<std::pair<std::vector<std::pair<int, long long>>, int>> &arbitrages)
{
	std::vector<int> u(num_alive);
	for (int i = 0; i < num_alive; i++)
		u[i] = i;

	#ifdef DEBUG
	int __count__ = 0;
	bool __dots__ = true;
	bool __flag__ = true;
	#endif

	// Get all possible quantity combinations
	long long num = 1;
	for (int i = 0; i < num_alive + 1; i++)
		num *= alive_orders[i].quantity;
	DEBUG_MSG(num << " quantity combinations\n");

	for (long long j = 0; j < num; j++) {
		long long tmp = j;
		std::vector<int> comb(num_alive + 1);
		for (int i = 0; i < num_alive + 1; i++) {
			comb[i] = 1 + tmp % alive_orders[i].quantity;
			tmp /= alive_orders[i].quantity;
		}

		DEBUG_MSG("Quantities: ");
		__print_vector(comb);
		DEBUG_MSG('\n');
		DEBUG_MSG("\tArbitrage\tProfit\tCycle\n");

		for (auto subset : powerset(u)) {
			subset.push_back(num_alive);
			std::unordered_map<std::string, long long> quantities;
			long long profit = 0;
			for (auto idx : subset) {
				switch (alive_orders[idx].type) {
				case 'b':
					for (auto it : alive_orders[idx].quantities)
						quantities[it.first] += comb[idx] * it.second;
					profit += comb[idx] * alive_orders[idx].price;
					break;
				case 's':
					for (auto it : alive_orders[idx].quantities)
						quantities[it.first] -= comb[idx] * it.second;
					profit -= comb[idx] * alive_orders[idx].price;
					break;
				}
			}

			bool flag = true;
			for (auto it : quantities) {
				if (it.second)
					flag = false;
			}
			if (flag && profit > 0) {
				std::vector<std::pair<int, long long>> v(subset.size());
				for (int i = 0; i < subset.size(); i++)
					v[i] = std::make_pair(subset[i], comb[subset[i]]);
				arbitrages.push_back(std::make_pair(v, profit));
			}

#ifdef DEBUG
			// if (j < 8) {
				if (flag)
					DEBUG_MSG("\tYes\t");
				else
					DEBUG_MSG("\tNo\t");
				DEBUG_MSG('\t' << profit << '\t');
				__print_vector(subset);
				DEBUG_MSG('\n');
			// } else {
				// if (__flag__) {
				// 	DEBUG_MSG("\t\t.\n\t\t.\n\t\t.\n");
				// 	__flag__ = false;
				// }
			// }
#endif

		}
	}

	if (arbitrages.size())
		return true;
	return false;
}

void part_3_sol(std::string &message, std::string &sp, int &num_alive,
                std::vector<order_q> &alive_orders, int &profit,
		std::unordered_map<std::string, long long> &alive_buy,
		std::unordered_map<std::string, long long> &alive_sell)
{
	int begin = 0;
	while (true) {
		int end = message.find('#', begin);
		if (end == std::string::npos) {
			if (begin != message.size())
				sp = message.substr(begin, message.size() - begin);
			break;
		}
		
		std::string curr = message.substr(begin, end - begin + 1);
		DEBUG_MSG("───────────────────────────────────────────────────\n");
		DEBUG_MSG(curr << "\n");

		if (sp.size() && begin == 0)
			curr = sp + curr;
		
		std::vector<std::string> stocks;
		std::string amount_str;
		std::string qty_str;
		long long   amount;
		long long   quantity;
		char        type;
		int         idx = -1;
		
		// Get linear combination
		DEBUG_MSG("Alive: " << num_alive << '\n');
		DEBUG_MSG("Stock\tQuantity\n");
		do {
			std::string stock_name;
			std::string qty_str;
			long long   quantity;

			while (curr[++idx] != ' ')
				stock_name += curr[idx];

			while (curr[++idx] != ' ') 
				qty_str += curr[idx];
			quantity = std::stoi(qty_str);
			
			// Resize vectors if necessary
			if (num_alive == alive_orders.size()) {
				DEBUG_MSG("#### PANIC ####");
				alive_orders.resize(2 * alive_orders.size());
			}

			alive_orders[num_alive].quantities[stock_name] = quantity;
			stocks.push_back(stock_name + qty_str);

			DEBUG_MSG(stock_name << '\t' << quantity << '\n');
		} while (curr[idx + 1] - '9' > 0);
		
		// Get price
		while (curr[++idx] != ' ')
			amount_str += curr[idx];
		amount = std::stoi(amount_str);
		alive_orders[num_alive].price = amount;
		DEBUG_MSG("Price: " << amount << '\n');

		// Get quantity
		while (curr[++idx] != ' ')
			qty_str += curr[idx];
		quantity = std::stoi(qty_str);
		DEBUG_MSG("Quantity: " << quantity << '\n');
		alive_orders[num_alive].quantity = quantity;

		// Get buy/sell
		type = curr[++idx];
		alive_orders[num_alive].type = type;
		DEBUG_MSG("Buy/sell: " << type << '\n');

		// Cancellation
		bool cancelled = false;
		for (int i = 0; i < num_alive; i++) {
			if (alive_orders[i].type != type &&
			    alive_orders[i].price == amount &&
			    alive_orders[i].name == alive_orders[num_alive].name) {
				if (alive_orders[i].quantity > quantity) {
					alive_orders[i].quantity -= quantity;
					alive_orders.erase(alive_orders.begin() + num_alive);
					cancelled = true;
					num_alive--;
					DEBUG_MSG("No trade\n");
					std::cout << "No trade\n";
					break;
				} else if (alive_orders[i].quantity == quantity) {
					alive_orders.erase(alive_orders.begin() + i);
					alive_orders.erase(alive_orders.begin() + num_alive);
					num_alive -= 2;
					cancelled = true;
					DEBUG_MSG("No trade\n");
					std::cout << "No trade\n";
					break;
				} else {
					alive_orders[num_alive].quantity -= alive_orders[i].quantity;
					alive_orders.erase(alive_orders.begin() + i);
					num_alive--;
				}
			}
		}

		if (cancelled) {
			DEBUG_MSG("No Trade\n");
			std::cout << "No Trade\n";
			num_alive++;
			begin = end + 2;
			if (begin > message.size() - 1)
				break;
			continue;
		}

		std::vector<std::pair<std::vector<std::pair<int, long long>>, int>> arbitrages;
		if (arbitrage(num_alive, alive_orders, arbitrages)) {
			int idx =
			        std::max_element(arbitrages.begin(),
			                         arbitrages.end(),
			                         [](const auto &l, const auto &r)
			                         { return l.second < r.second; })
			            - arbitrages.begin();
			for (int i = arbitrages[idx].first.size() - 1; i >= 0; i--) {
				// Print traded orders
				for (auto it : alive_orders[arbitrages[idx].first[i].first].quantities) {
					DEBUG_MSG(it.first << ' ' << it.second << ' ');
					std::cout << it.first << ' ' << it.second << ' ';
				}
				DEBUG_MSG(alive_orders[i].price << ' ');
				DEBUG_MSG(arbitrages[idx].first[i].second << ' ');
				DEBUG_MSG(char('b' + 's' - alive_orders[i].type) << "#\n");
				std::cout << alive_orders[i].price << ' ';
				std::cout << arbitrages[idx].first[i].second << ' ';
				std::cout << char('b' + 's' - alive_orders[i].type) << "#\n";

				// Remove traded orders
				alive_orders[arbitrages[idx].first[i].first].quantity -=
					arbitrages[idx].first[i].second;
				if (alive_orders[arbitrages[idx].first[i].first].quantity == 0) {
					alive_orders.erase(alive_orders.begin() +
					                   arbitrages[idx].first[i].first);
					num_alive--;
				}
			}
			profit += arbitrages[idx].second;
			DEBUG_MSG("Profit: " << profit << '\n');
		} else {
			DEBUG_MSG("No Trade\n");
			std::cout << "No Trade\n";
		}

		num_alive++;
		begin = end + 2;
		if (begin > message.size() - 1)
			break;
	}
}

void part_3(Receiver rcv)
{
	bool input_end = false;
	std::string sp = "";

	// Initialize to 64 to prevent frequent resize && copy operations
	std::vector<order_q> alive_orders(64);
	std::unordered_map<std::string, long long> alive_buy;
	std::unordered_map<std::string, long long> alive_sell;
	int num_alive = 0;
	int profit    = 0;

	while (!input_end) {
		std::string message = rcv.readIML();
		part_3_sol(message, sp, num_alive, alive_orders, profit,
		           alive_buy, alive_sell);

		if (message.find('$') != std::string::npos) {
			rcv.terminate();

			// Print profit at end
			std::cout << profit << '\n';

			DEBUG_MSG("───────────────────────────────────────────────────\n");
			input_end = true;
		}
	}
}

		// switch (type) {
		// case 'b':
		// 	// Combine buy orders
		// 	if (alive_buy.contains(alive_orders[num_alive].name) &&
		// 	    alive_buy[alive_orders[num_alive].name] == amount) {
		// 		int idx = 0;
		// 		for (idx = 0; idx < num_alive; idx++) {
		// 			if (alive_orders[idx].name == name &&
		// 			    alive_orders[idx].type == 'b')
		// 				break;
		// 		}
		// 		alive_orders[num_alive].quantity += alive_orders[idx].quantity;
		// 		alive_orders.erase(alive_orders.begin() + idx);
		// 		num_alive--;
		// 	}
		// 	// Cancel buy/sell
		// 	if (alive_sell.contains(alive_orders[num_alive].name) &&
		// 	    alive_sell[alive_orders[num_alive].name] == amount) {
		// 		int idx = 0;
		// 		for (idx = 0; idx < num_alive; idx++) {
		// 			if (alive_orders[idx].name == name &&
		// 			    alive_orders[idx].type == 's')
		// 				break;
		// 		}
		// 		if (alive_orders[num_alive].quantity > alive_orders[idx].quantity) {
		// 			alive_orders[num_alive].quantity -= alive_orders[idx].quantity;
		// 			alive_orders.erase(alive_orders.begin() + idx);
		// 			num_alive--;
		// 			break;
		// 		}
		// 		if (alive_orders[num_alive].quantity < alive_orders[idx].quantity) {
		// 			alive_orders[idx].quantity -= alive_orders[num_alive].quantity;
		// 			alive_orders.erase(alive_orders.begin() + num_alive);
		// 			num_alive--;
		// 			cancelled = true;
		// 			break;
		// 		}
		// 		alive_orders.erase(alive_orders.begin() + num_alive);
		// 		alive_orders.erase(alive_orders.begin() + idx);
		// 		num_alive -= 2;
		// 		cancelled = true;
		// 		break;
		// 	}
		// case 's':
		// 	if (alive_sell.contains(alive_orders[num_alive].name) &&
		// 	    alive_sell[alive_orders[num_alive]] == amount) {
		// 		int idx = 0;
		// 		for (idx = 0; idx < num_alive; idx++) {
		// 			if (alive_orders[idx].name == name)
		// 				break;
		// 		}
		// 		alive_orders[num_alive].quantity += alive_orders[idx].quantity;
		// 		alive_orders.erase(alive_orders.begin() + idx);
		// 		num_alive--;
		// 	}
		// 	if (alive_buy.contains(alive_orders[num_alive].name) &&
		// 	    alive_buy[alive_orders[num_alive]] == amount) {
		// 		int idx = 0;
		// 		for (idx = 0; idx < num_alive; idx++) {
		// 			if (alive_orders[idx].name == name)
		// 				break;
		// 		}
		// 		if (alive_orders[num_alive].quantity >
		// 			alive_buy[alive_orders[num_alive].name]) {
		// 			alive_orders[num_alive].quantity -= alive_orders[idx].quantity;
		// 			alive_orders.erase(alive_orders.begin() + idx);
		// 			num_alive--;
		// 			break;
		// 		}
		// 		if (alive_orders[num_alive].quantity <
		// 			alive_sell[alive_orders[num_alive].name]) {
		// 			alive_orders[idx].quantity -= alive_orders[num_alive].quantity;
		// 			alive_orders.erase(alive_orders.begin() + num_alive);
		// 			num_alive--;
		// 			break;
		// 		}
		// 		alive_orders.erase(alive_orders.begin() + num_alive);
		// 		alive_orders.erase(alive_orders.begin() + idx);
		// 		num_alive -= 2;
		// 		break;
		// 	}
		// 	break;
		// }
