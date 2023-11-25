#include "../stl/algorithm"

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

		long long num = 1;	// number of quantity combinations
		for (auto idx : subset)
			num *= lin_combs[idx].second;

		#ifdef DEBUG
		bool __flag__ = true;
		#endif

		for (int j = 0; j < num; j++) {
			long long tmp = j;
			std::vector<int> comb(subset.size());

			for (int i = 0; i < subset.size(); i++) {
				comb[i] = 1 + tmp % lin_combs[subset[i]].second;
				tmp /= lin_combs[subset[i]].second;
			}

			for (auto idx = subset.begin(); idx < subset.end(); idx++) {
				for (auto it = lin_combs[*idx].first.begin();
					it != lin_combs[*idx].first.end(); it++) {
					if (prices[*idx].second == 'b')
						lin_comb[it->first] += comb[idx - subset.begin()] * it->second;
					else
						lin_comb[it->first] -= comb[idx - subset.begin()] * it->second;
				}
				if (prices[*idx].second == 'b')
					profit += comb[idx - subset.begin()] * prices[*idx].first;
				else
					profit -= comb[idx - subset.begin()] * prices[*idx].first;
			}

			#ifdef DEBUG
			if (j < 8) {
				DEBUG_MSG("\t\tCombination " << j << ": ");
				__print_vector(comb);
				DEBUG_MSG("\tProfit: " << profit << '\n');
			} else {
				if (__flag__) {
					DEBUG_MSG("\t\t.\n\t\t.\n\t\t.\n");
					__flag__ = false;
				}
			}
			#endif

			bool flag = true;
			for (auto it = lin_comb.begin(); it != lin_comb.end(); it++) {
				if (it->second) {
					flag = false;
				}
			}
			if (flag && profit > 0) {
				std::vector<std::pair<int, long long>> v(subset.size());
				for (int i = 0; i < subset.size(); i++) {
					v[i] = std::make_pair(subset[i], comb[i]);
				}
				arbitrages.push_back(std::make_pair(v, profit));
			}
		}
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
				DEBUG_MSG("#### PANIC ####");
				lin_combs.resize(2 * lin_combs.size());
				prices.resize(2 * prices.size());
			}

			lin_combs[num_alive].first[stock_name] = quantity;

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
		lin_combs[num_alive].second = quantity;

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
				lin_combs[arbitrages[idx].first[i].first].second -= arbitrages[idx].first[i].second;
				if (lin_combs[arbitrages[idx].first[i].first].second == 0) {
					lin_combs.erase(lin_combs.begin() + arbitrages[idx].first[i].first);
					prices.erase(prices.begin() + arbitrages[idx].first[i].first);
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
	std::vector<std::pair<std::unordered_map<std::string, long long>, long long>> lin_combs(64);
	std::vector<std::pair<long long, char>> prices(64);
	int num_alive = 0;
	int profit    = 0;

	while (!input_end) {
		std::string message = rcv.readIML();
		part_3_sol(message, sp, num_alive, lin_combs, prices, profit);

		if (message.find('$') != std::string::npos) {
			rcv.terminate();

			// Print profit at end
			std::cout << profit << '\n';

			DEBUG_MSG("───────────────────────────────────────────────────\n");
			input_end = true;
		}
	}
}
