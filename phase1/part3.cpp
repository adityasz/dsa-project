#include <algorithm>

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

bool arbitrage(
	int num_alive,
        std::vector<std::pair<std::unordered_map<std::string, long long>, long long>> &lin_combs,
        std::vector<std::pair<long long, char>> &prices,
	std::vector<std::pair<std::vector<std::pair<int, long long>>, int>> &arbitrages)
{
	std::vector<int> u(num_alive);
	for (int i = 0; i < num_alive; i++)
		u[i] = i;
	DEBUG_MSG("Cycles:\n");
	for (auto subset : powerset(u)) {
		std::unordered_map<std::string, long long> lin_comb;
		long long profit = 0;
		subset.push_back(num_alive);

		DEBUG_MSG('\t');
		__print_vector(subset);
		DEBUG_MSG('\n');

		for (auto idx : subset) {
			for (auto it = lin_combs[idx].begin(); it != lin_combs[idx].end(); it++) {
				if (prices[idx].second == 'b')
					lin_comb[it->first] += it->second;
				else
					lin_comb[it->first] -= it->second;
			}
			if (prices[idx].second == 'b')
				profit += prices[idx].first;
			else
				profit -= prices[idx].first;
		}
		bool flag = true;
		for (auto it = lin_comb.begin(); it != lin_comb.end(); it++) {
			if (it->second) {
				flag = false;
			}
		}
		if (flag && profit > 0)
			arbitrages.push_back(std::make_pair(subset, profit));
	}
	if (arbitrages.size())
		return true;
	return false;
}
void part_3_sol(std::string &message, std::string &sp, int &num_alive,
	std::vector<std::pair<std::unordered_map<std::string, long long>, long long>> &lin_combs,
	std::vector<std::pair<long long, char>> &prices, int &profit)
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
		
		std::string amount_str;
		std::string qty_str;
		long long   amount;
		long long   quantity;
		char        buy_sell;
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
			if (num_alive == lin_combs.size()) {
				lin_combs.resize(2 * lin_combs.size());
				prices.resize(2 * prices.size());
			}

			lin_combs[num_alive][stock_name] = quantity;

			DEBUG_MSG(stock_name << '\t' << quantity << '\n');
		} while (curr[idx + 1] - '9' > 0);
		
		// Get price
		while (curr[++idx] != ' ')
			amount_str += curr[idx];
		amount = std::stoi(amount_str);
		DEBUG_MSG("Price: " << amount << '\n');

		// Get quantity
		while (curr[++idx] != ' ')
			qty_str += curr[idx];
		quantity = std::stoi(qty_str);
		DEBUG_MSG("Quantity: " << quantity << '\n');

		// Get buy/sell
		buy_sell = curr[++idx];
		DEBUG_MSG("Buy/sell: " << buy_sell << '\n');
		
		prices[num_alive] = std::make_pair(amount, buy_sell);

		std::vector<std::pair<std::vector<std::pair<int, long long>>, int>> arbitrages;
		if (arbitrage(num_alive, lin_combs, prices, arbitrages)) {
			int idx =
			        std::max_element(arbitrages.begin(),
			                         arbitrages.end(),
			                         [](const auto &l, const auto &r)
			                         { return l.second < r.second; })
			            - arbitrages.begin();
			for (int i = arbitrages[idx].first.size() - 1; i >= 0; i--) {
				// Print traded orders
				for (auto it = lin_combs[arbitrages[idx].first[i].first].first.begin();
				     it != lin_combs[arbitrages[idx].first[i].first].first.end(); it++) {
					DEBUG_MSG(it->first << ' ' << it->second << ' ');
					std::cout << it->first << ' ' << it->second << ' ';
				}
				DEBUG_MSG(prices[i].first << ' ');
				DEBUG_MSG(arbitrages[idx].first[i].second << ' ');
				DEBUG_MSG(char('b' + 's' - prices[i].second) << "#\n");
				std::cout << prices[i].first << ' ';
				std::cout << arbitrages[idx].first[i].second << ' ';
				std::cout << char('b' + 's' - prices[i].second) << "#\n";

				// Remove traded orders
				lin_combs.erase(lin_combs.begin() + arbitrages[idx].first[i]);
				prices.erase(prices.begin() + arbitrages[idx].first[i]);
				num_alive--;
			}
			DEBUG_MSG(profit);
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

void part_3(Receiver rcv)
{
	bool input_end = false;
	std::string sp = "";

	// Initialize to 64 to prevent frequent resize && copy operations
	std::vector<std::pair<std::unordered_map<std::string, long long>, long long>> lin_combs(64);
	std::vector<std::pair<long long, char>> prices(64);
	int num_alive = 0;
	int profit    = 0;

	while (!input_end) {
		std::string message = rcv.readIML();
		part_3_sol(message, sp, num_alive, lin_combs, prices, profit);

		if (message.find('$') != std::string::npos) {
			rcv.terminate();
			DEBUG_MSG("───────────────────────────────────────────────────\n");
			input_end = true;
		}
	}
}
