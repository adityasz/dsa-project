#include <algorithm>

std::vector<std::vector<int>> powerset(std::vector<int> &v)
{
	unsigned int n = v.size();
	std::vector<std::vector<int>> powerset(1 << n);
	for (unsigned long long i = 0; i < static_cast<unsigned int>(1 << n); i++) {
		std::vector<int> subset;
		for (unsigned long long j = 0; j < n; j++) {
			if (static_cast<unsigned long long>(i >> j) & 1)
				subset.push_back(v[j]);
		}
		powerset[i] = subset;
	}
	return powerset;
}

bool arbitrage(
	int num_alive,
        std::vector<std::unordered_map<std::string, long long>> &quantities,
        std::vector<std::pair<long long, char>> &prices,
	std::vector<std::pair<std::vector<int>, int>> &arbitrages)
{
	std::vector<int> u(num_alive);
	for (int i = 0; i < num_alive; i++)
		u[i] = i;
	std::cerr << "Subsets:\n";
	for (auto subset : powerset(u)) {
		std::unordered_map<std::string, long long> qty;
		long long profit = 0;
		subset.push_back(num_alive);

		std::cerr << '\t';
		for (auto x : subset) {
			std::cerr << x << ' ';
		}
		std::cerr << '\n';

		for (auto idx : subset) {
			for (auto it = quantities[idx].begin(); it != quantities[idx].end(); it++) {
				if (prices[idx].second == 'b')
					qty[it->first] += it->second;
				else
					qty[it->first] -= it->second;
			}
			if (prices[idx].second == 'b')
				profit += prices[idx].first;
			else
				profit -= prices[idx].first;
		}
		bool flag = true;
		for (auto it = qty.begin(); it != qty.end(); it++) {
			if (it->second) {
				flag = false;
			}
		}
		if (flag)
			arbitrages.push_back(std::make_pair(subset, profit));
	}
	if (arbitrages.size())
		return true;
	return false;
}

void part_2_sol(std::string &message, std::string &sp, int &num_alive,
        std::vector<std::unordered_map<std::string, long long>> &quantities,
        std::vector<std::pair<long long, char>> &prices)
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
		std::cerr << "───────────────────────────────────────────────────\n";
		std::cerr << curr << "\n";

		if (sp.size() && begin == 0)
			curr = sp + curr;
		
		std::string amount_str;
		long long   amount;
		char        buy_sell;
		int         idx = -1;
		
		// Get linear combination
		std::cerr << "Line: " << num_alive << '\n';
		std::cerr << "Stock\tQuantity\n";
		do {
			std::string stock_name;
			std::string qty_str;
			long long   quantity;

			while (curr[++idx] != ' ')
				stock_name += curr[idx];

			while (curr[++idx] != ' ') 
				qty_str += curr[idx];
			quantity = std::stoi(qty_str);
			
			quantities[num_alive][stock_name] = quantity;

			std::cerr << stock_name << '\t' << quantity << '\n';
		} while (curr[idx + 1] - '9' > 0);
		
		// Get price
		while (curr[++idx] != ' ')
			amount_str += curr[idx];
		amount = std::stoi(amount_str);
		std::cerr << "Price: " << amount << '\n';

		// Get buy/sell
		buy_sell = curr[++idx];
		std::cerr << "Buy/sell: " << buy_sell << '\n';
		
		prices[num_alive] = std::make_pair(amount, buy_sell);

		std::vector<std::pair<std::vector<int>, int>> arbitrages;
		if (arbitrage(num_alive, quantities, prices, arbitrages)) {
			int idx =
			        std::max_element(arbitrages.begin(),
			                         arbitrages.end(),
			                         [](const auto &l, const auto &r)
			                         { return l.second < r.second; })
			            - arbitrages.begin();
			for (int i = arbitrages[idx].first.size() - 1; i >= 0; i--) {
				for (auto it = quantities[arbitrages[idx].first[i]].begin();
				     it != quantities[arbitrages[idx].first[i]].end(); it++) {
					std::cout << it->first << ' ' << it->second << ' ';
				}
				std::cout << prices[i].first << ' ';
				std::cout << char('b' + 's' - prices[i].second) << "#\n";
			}
			std::cout << arbitrages[idx].second << '\n';
			for (auto x : arbitrages[idx].first) {
				quantities.erase(quantities.begin() + x);
				num_alive--;
			}
		} else {
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
	std::vector<std::unordered_map<std::string, long long>> quantities(64);
	std::vector<std::pair<long long, char>>                 prices(64);
	int num_alive = 0;

	while (!input_end) {
		std::string message = rcv.readIML();
		part_2_sol(message, sp, num_alive, quantities, prices);

		if (message.find('$') != std::string::npos) {
			rcv.terminate();
			std::cerr << "───────────────────────────────────────────────────\n";
			input_end = true;
		}
	}
}
