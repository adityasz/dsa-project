void solution(std::string message, std::string &sp)
{
	// TODO: this doesn't keep track of trades across messages
	std::unordered_map<std::string, int> last_trade;
	std::unordered_map<std::string, int> alive_buy;
	std::unordered_map<std::string, int> alive_sell;

	int begin = 0;
	while (true) {
		int end = message.find('\r', begin);
		if (end == -1) {
			if (begin != message.size())
				sp = message.substr(begin, message.size() - begin);
			break;
		}

		std::string curr = message.substr(begin, end - begin);

		if (sp.size() && begin == 0)
			curr = sp + curr;

		// Extract stock name
		int idx = curr.find(' ');
		std::string stock_name = message.substr(0, idx - 1);
		begin = idx + 1;

		// Extract amount
		idx = message.find(' ', begin);
		std::string amountstr = message.substr(begin, idx - begin);
		int amount = std::stoi(amountstr);
		begin = idx + 1;

		// Extract buy/sell type
		idx = message.find('#', begin);
		std::string buy_sell = message.substr(begin, idx - begin);

		if (!last_trade.contains(stock_name)) {
			last_trade[stock_name] = amount;
			alive_buy[stock_name] = -1;
			alive_sell[stock_name] = -1;
			if (buy_sell == "b")
				std::cout << stock_name << " " << amount << " " << "s" << "\r\n";
			else
				std::cout << stock_name << " " << amount << " " << "b" << "\r\n";
			continue;
		}

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

		begin = end + 1;
	}
}
