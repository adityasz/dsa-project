#include "stl/algorithm"

struct order_q {
	std::unordered_map<std::string, int> quantities;
	std::string name;
	int  price;
	char type;
	int  quantity;
};

bool arbitrage(int num_alive, std::vector<order_q> &alive_orders,
               std::vector<std::pair<std::vector<std::pair<int, int>>, int>> &arbitrages)
{
	std::vector<int> u(num_alive);
	for (int i = 0; i < num_alive; i++)
		u[i] = i;

	for (auto subset : powerset(u)) {
		subset.push_back(num_alive);

		// Get all possible quantity combinations
		int num = 1;
		for (auto idx : subset)
			num *= alive_orders[idx].quantity;
		DEBUG_MSG(num << " quantity combinations\n");

		for (int j = 0; j < num; j++) {
			std::unordered_map<std::string, int> quantities;
			int profit = 0;
			int tmp = j;
			std::vector<int> comb(subset.size());

			for (int i = 0; i < subset.size(); i++) {
				comb[i] = 1 + tmp % alive_orders[subset[i]].quantity;
				tmp /= alive_orders[subset[i]].quantity;
			}

			for (auto idx = subset.begin(); idx < subset.end(); idx++) {
				switch (alive_orders[*idx].type) {
				case 'b':
					for (auto it = alive_orders[*idx].quantities.begin();
					     it != alive_orders[*idx].quantities.end(); it++)
						quantities[it->first] += comb[idx - subset.begin()] * it->second;
					profit += comb[idx - subset.begin()] * alive_orders[*idx].price;
					break;
				case 's':
					for (auto it = alive_orders[*idx].quantities.begin();
					     it != alive_orders[*idx].quantities.end(); it++)
						quantities[it->first] -= comb[idx - subset.begin()] * it->second;
					profit -= comb[idx - subset.begin()] * alive_orders[*idx].price;
					break;
				}
			}

			bool flag = true;
			for (auto it : quantities) {
				if (it.second)
					flag = false;
			}
			if (flag && profit > 0) {
				std::vector<std::pair<int, int>> v(subset.size());
				for (int i = 0; i < subset.size(); i++)
					v[i] = std::make_pair(subset[i], comb[i]);
				arbitrages.push_back(std::make_pair(v, profit));
			}
#ifdef DEBUG
			if (flag) {
				DEBUG_MSG("Quantities: ");
				__print_vector(comb);
				DEBUG_MSG('\n');
				DEBUG_MSG("\tArbitrage\tProfit\tCycle\n");
				DEBUG_MSG("\tYes\t");
				DEBUG_MSG('\t' << profit << '\t');
				__print_vector(subset);
				DEBUG_MSG('\n');
			}
#endif
		}
	}

	if (arbitrages.size())
		return true;
	return false;
}

void part_3_sol(std::string &message, std::string &sp, int &num_alive,
                std::vector<order_q> &alive_orders, int &profit)
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
		int         amount;
		int         quantity;
		char        type;
		int         idx = -1;
		
		// Get linear combination
		DEBUG_MSG("Alive: " << num_alive << '\n');
		DEBUG_MSG("Stock\tQuantity\n");
		do {
			std::string stock_name;
			std::string qty_str;
			int   quantity;

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

		// Set name
		std::sort(stocks.begin(), stocks.end());
		for (auto stock : stocks)
			alive_orders[num_alive].name += stock;
		DEBUG_MSG("Name: " << alive_orders[num_alive].name << '\n');

		// Cancellation
		bool cancelled = false;
loop_begin:
		for (int i = 0; i < num_alive; i++) {
			if (alive_orders[i].type != type &&
			    alive_orders[i].price == amount &&
			    alive_orders[i].name == alive_orders[num_alive].name) {
				if (alive_orders[i].quantity > quantity) {
					alive_orders[i].quantity -= quantity;
					alive_orders.erase(alive_orders.begin() + num_alive);
					num_alive--;
					DEBUG_MSG("Cancelled due to 1\n");
					cancelled = true;
					break;
				} else if (alive_orders[i].quantity == quantity) {
					alive_orders.erase(alive_orders.begin() + i);
					num_alive--;
					alive_orders.erase(alive_orders.begin() + num_alive);
					num_alive--;
					DEBUG_MSG("Cancelled due to 2\n");
					cancelled = true;
					break;
				} else {
					alive_orders[num_alive].quantity -= alive_orders[i].quantity;
					alive_orders.erase(alive_orders.begin() + i);
					num_alive--;
					goto loop_begin;
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

		std::vector<std::pair<std::vector<std::pair<int, int>>, int>> arbitrages;
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
	int num_alive = 0;
	int profit    = 0;

	while (!input_end) {
		std::string message = rcv.readIML();
		part_3_sol(message, sp, num_alive, alive_orders, profit);

		if (message.find('$') != std::string::npos) {
			rcv.terminate();

			// Print profit at end
			std::cout << profit << '\n';

			DEBUG_MSG("───────────────────────────────────────────────────\n");
			input_end = true;
		}
	}
}
