#define INVALID -9223372036854775807

void part_1_sol(std::string message, std::string &sp,
                std::unordered_map<std::string, long long> &last_trade,
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
		std::cerr << "───────────────────────────────────────────────────\n";
		std::cerr << curr << "\n";

		if (sp.size() && begin == 0)
			curr = sp + curr;
		
		std::string stock_name;
		std::string amount_str;
		long long   amount;
		char        buy_sell;
		int         idx = -1;

		// Get stock name
		while (curr[++idx] != ' ') {
			stock_name += curr[idx];
		}

		// Get amount
		while (curr[++idx] != ' ') {
			amount_str += curr[idx];
		}
		amount = std::stoi(amount_str);

		// Get buy/sell
		buy_sell = curr[++idx];

		std::cerr << "Stock name: " << stock_name
		          << "\tAmount: "   << amount
		          << "\tBuy/sell: " << buy_sell << "\n";

		if (!last_trade.contains(stock_name)) {
			last_trade[stock_name] = amount;
			alive_buy[stock_name] = INVALID;
			alive_sell[stock_name] = INVALID;

			if (buy_sell == 'b')
				std::cout << stock_name << " " << amount
				          << " " << 's' << "\r\n";
			else
				std::cout << stock_name << " " << amount
				          << " " << 'b' << "\r\n";

			begin = end + 2;
			if (begin > message.size() - 1)
				break;
			continue;
		}

		long long &ab   = alive_buy[stock_name];
		long long &as   = alive_sell[stock_name];
		long long &prev = last_trade[stock_name];

		if (buy_sell == 'b') {
			if (ab != INVALID && ab >= amount) {
				std::cout << "No Trade\r\n";
			} else if (as == amount) {
				ab = INVALID;
				as = INVALID;
				std::cout << "No Trade\r\n";
			} else {
				ab = INVALID;
				if (amount > prev) {
					std::cout << stock_name << " " << amount
					          << " " << "s" << "\r\n";
					prev = amount;
				} else {
					std::cout << "No Trade\r\n";
					ab = amount;
				}
			}
		} else {
			if (as != INVALID && as <= amount) {
				std::cout << "No Trade\r\n";
			} else if (ab == amount) {
				ab = INVALID;
				as = INVALID;
				std::cout << "No Trade\r\n";
			} else {
				as = INVALID;
				if (amount < prev) {
					std::cout << stock_name << " " << amount
					          << " " << "b" << "\r\n";
					prev = amount;
				} else {
					std::cout << "No Trade\r\n";
					as = amount;
				}
			}
		}

		begin = end + 2;
		if (begin > message.size() - 1)
			break;
	}
}

void part_1(Receiver rcv)
{
	bool input_end = false;
	std::string sp = "";

	std::unordered_map<std::string, long long> last_trade;
	std::unordered_map<std::string, long long> alive_buy;
	std::unordered_map<std::string, long long> alive_sell;

	while (!input_end) {
		std::string message = rcv.readIML();
		part_1_sol(message, sp, last_trade, alive_buy, alive_sell);

		if (message.find('$') != std::string::npos) {
			rcv.terminate();
			std::cerr << "───────────────────────────────────────────────────\n";
			input_end = true;
		}
	}
}
