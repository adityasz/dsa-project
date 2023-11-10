void solution(std::string message, std::string &sp,
              std::unordered_map<std::string, int> &last_trade,
              std::unordered_map<std::string, int> &alive_buy,
              std::unordered_map<std::string, int> &alive_sell)
{
	int begin = 0;
	int hello = 0;
	while (true) {
		int end = message.find('\r', begin);
		if (end == -1) {
			if (begin != message.size())
				sp = message.substr(begin, message.size() - begin - 1);
			break;
		}

		std::string curr = message.substr(begin, end - begin - 1);
		std::cerr << curr << "\n";

		if (sp.size() && begin == 0)
			curr = sp + curr;

		// Extract stock name
		int i = 0;
		int j = curr.find(' ');
		std::string stock_name = curr.substr(i, j);
		i = j + 1;

		// Extract amount
		j = curr.find(' ', i);
		// std::string amountstr = curr.substr(begin, idx - begin - 1);
		std::string amountstr = curr.substr(i, j);
		int amount = std::stoi(amountstr);
		i = j + 1;

		// Extract buy/sell type
		j = message.find('#', i);
		std::string buy_sell = curr.substr(i, j - i);

		// TODO: fix amount string
		std::cerr << "Stock name: " << stock_name << ", amount: " << amountstr << ", buy/sell: " << buy_sell << "\n";

		if (!last_trade.contains(stock_name)) {
			last_trade[stock_name] = amount;
			alive_buy[stock_name] = -1;
			alive_sell[stock_name] = -1;
			if (buy_sell == "b")
				std::cout << stock_name << " " << amount << " " << "s" << "\r\n";
			else
				std::cout << stock_name << " " << amount << " " << "b" << "\r\n";
			// continue;
		} else {
			int &ab = alive_buy[stock_name];
			int &as = alive_sell[stock_name];

			if (buy_sell == "b") {
				if (ab != -1 && ab >= amount) {
					std::cout << "No Trade\r\n";
				} else if (as != -1 && as == amount) {
					ab = -1;
					as = -1;
					std::cout << "No Trade\r\n";
				} else {
					ab = -1;
					if (amount > last_trade[stock_name]) {
						std::cout << stock_name << " " << amount << " " << "s" << "\r\n";
						last_trade[stock_name] = amount;
					} else {
						std::cout << "No Trade\r\n";
						ab = amount;
					}
				}
			} else {
				if (as != -1 && as <= amount) {
					std::cout << "No Trade\r\n";
				} else if (ab != -1 && ab == amount) {
					ab = -1;
					as = -1;
					std::cout << "No Trade\r\n";
				} else {
					as = -1;
					if (amount < last_trade[stock_name]) {
						std::cout << stock_name << " " << amount << " " << "b" << "\r\n";
						last_trade[stock_name] = amount;
					} else {
						std::cout << "No Trade\r\n";
						as = amount;
					}
				}
			}
		}

		begin = end + 1;
	}
}
