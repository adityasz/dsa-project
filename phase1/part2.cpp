// TODO: Instead of returning powerset, find arbitrage...
std::vector<std::vector<int>> powerset(std::vector<int> v)
{
	std::vector<std::vector<int>> powerset;
	std::vector<int> emptyset;
	powerset.push_back(emptyset);
	for (int i = 0; i < v.size(); i++) {
		std::vector<std::vector<int>> subsets = powerset;

		for (int j = 0; j < subsets.size(); j++)
			subsets[j].push_back(v[i]);

		for (int j = 0; j < subsets.size(); j++)
			powerset.push_back(subsets[j]);
	}
	return powerset;
}

void part_2_sol(std::string &message, std::string &sp, int &line,
        std::vector<std::unordered_map<std::string, long long>> &quantities,
        std::vector<std::pair<long long, char>>                 &prices)
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
		do {
			std::string stock_name;
			std::string qty_str;
			long long   quantity;

			while (curr[++idx] != ' ')
				stock_name += curr[idx];

			while (curr[++idx] != ' ') 
				qty_str += curr[idx];
			quantity = std::stoi(qty_str);
			
			quantities[line][stock_name] = quantity;

			std::cerr << stock_name << "\t" << quantity << "\n";
		} while (curr[idx + 1] - '9' > 0);
		
		// Get price
		while (curr[++idx] != ' ')
			amount_str += curr[idx];
		amount = std::stoi(amount_str);
		std::cerr << amount << "\n";

		// Get buy/sell
		buy_sell = curr[++idx];
		std::cerr << buy_sell << "\n";
		
		prices.push_back(std::make_pair(amount, buy_sell));

		line++;
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
	int line = 0;

	while (!input_end) {
		std::string message = rcv.readIML();
		part_2_sol(message, sp, line, quantities, prices);

		if (message.find('$') != std::string::npos) {
			rcv.terminate();
			std::cerr << "───────────────────────────────────────────────────\n";
			input_end = true;
		}
	}
}
