#include "stl/algorithm"

struct order {
	std::unordered_map<std::string, long long> quantities;
	int         price;
	char        type;
	std::string name;
};

void powerset_helper(std::vector<int> &v, int i, std::vector<int> &subset,
		     std::vector<std::vector<int>> &powerset)
{
	powerset.push_back(subset);
	for (int j = i; j < v.size(); j++) {
		subset.push_back(v[j]);
		powerset_helper(v, j + 1, subset, powerset);
		subset.pop_back();
	}
}

std::vector<std::vector<int>> powerset(std::vector<int> &v)
{
	std::vector<std::vector<int>> powerset;
	std::vector<int> subset;
	powerset_helper(v, 0, subset, powerset);
	return powerset;
}

bool arbitrage(int num_alive, std::vector<order> &alive_orders,
               std::vector<std::pair<std::vector<int>, int>> &arbitrages)
{
	std::vector<int> u(num_alive);
	for (int i = 0; i < num_alive; i++)
		u[i] = i;

	DEBUG_MSG("Cycles:\n");
	#ifdef DEBUG
	int __count__ = 0;
	bool __dots__ = true;
	#endif

	for (auto subset : powerset(u)) {
		std::unordered_map<std::string, long long> lin_comb;
		long long profit = 0;
		subset.push_back(num_alive);

		#ifdef DEBUG
		if (__count__++ < 8) {
			DEBUG_MSG('\t');
			__print_vector(subset);
			DEBUG_MSG('\n');
		} else {
			if (__dots__) {
				DEBUG_MSG("\t...\n");
				__dots__ = false;
			}
		}
		#endif

		for (auto idx : subset) {
			switch (alive_orders[idx].type) {
			case 'b':
				for (auto it : alive_orders[idx].quantities) {
					lin_comb[it.first] += it.second;
				}
				profit += alive_orders[idx].price;
				break;
			case 's':
				for (auto it : alive_orders[idx].quantities) {
					lin_comb[it.first] -= it.second;
				}
				profit -= alive_orders[idx].price;
				break;
			}
		}

		bool flag = true;
		for (auto it : lin_comb) {
			if (it.second)
				flag = false;
		}

		if (flag && profit > 0)
			arbitrages.push_back(std::make_pair(subset, profit));
	}

	if (arbitrages.size())
		return true;
	return false;
}

void part_2_sol(std::string &message, std::string &sp, int &num_alive,
                std::vector<order> &alive_orders, int &profit,
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
		long long   amount;
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
		DEBUG_MSG("Price: " << amount << '\n');
		alive_orders[num_alive].price = amount;

		// Get buy/sell
		type = curr[++idx];
		DEBUG_MSG("Type: " << type << '\n');
		alive_orders[num_alive].type = type;

		std::sort(stocks.begin(), stocks.end());
		for (auto stock : stocks)
			alive_orders[num_alive].name += stock;
		DEBUG_MSG("Name: " << alive_orders[num_alive].name << '\n');

		std::vector<std::pair<std::vector<int>, int>> arbitrages;

		if (arbitrage(num_alive, alive_orders, arbitrages)) {
			int idx =
			        std::max_element(arbitrages.begin(),
			                         arbitrages.end(),
			                         [](const auto &l, const auto &r)
			                         { return l.second < r.second; })
			            - arbitrages.begin();

			// Print traded orders
			for (int i = arbitrages[idx].first.size() - 1; i >= 0; i--) {
				for (auto it : alive_orders[arbitrages[idx].first[i]].quantities) {
					DEBUG_MSG(it.first << ' ' << it.second << ' ');
					std::cout << it.first << ' ' << it.second << ' ';
				}
				DEBUG_MSG(alive_orders[arbitrages[idx].first[i]].price << ' ');
				DEBUG_MSG(char('b' + 's' - alive_orders[arbitrages[idx].first[i]].type)
				          << "#\n");
				std::cout << alive_orders[arbitrages[idx].first[i]].price << ' ';
				std::cout << char('b' + 's' - alive_orders[arbitrages[idx].first[i]].type)
				          << "#\n";

				// Remove traded orders
				alive_orders.erase(alive_orders.begin() + arbitrages[idx].first[i]);
				num_alive--;
			}

			DEBUG_MSG("Profit: " << arbitrages[idx].second << '\n');
			profit += arbitrages[idx].second;
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

void part_2(Receiver rcv)
{
	bool input_end = false;
	std::string sp = "";

	// Initialize to 64 to prevent frequent resize && copy operations
	std::vector<order> alive_orders(64);
	std::unordered_map<std::string, long long> alive_buy;
	std::unordered_map<std::string, long long> alive_sell;
	int num_alive = 0;
	int profit    = 0;

	while (!input_end) {
		std::string message = rcv.readIML();
		part_2_sol(message, sp, num_alive, alive_orders, profit,
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
