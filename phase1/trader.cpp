#include <string>
#include <cassert>
#include <unordered_map>

#include "receiver.h"
#include "part1.cpp"

int main()
{
	Receiver rcv;
	// sleep(5);
	// std::string message = rcv.readIML();
	// std::cout << message;

	bool input_end = false;
	std::string sp = "";
	std::unordered_map<std::string, int> last_trade;
	std::unordered_map<std::string, int> alive_buy;
	std::unordered_map<std::string, int> alive_sell;

	// int i = 0;

	while (!input_end) {
		std::string message = rcv.readIML();
		solution(message, sp, last_trade, alive_buy, alive_sell);

		// i++;
		// std::cerr << "Iteration: " << i << "\n===\n";
		// std::cerr << "Size: " << message.size() << "\n";

		// for (int j = 0; j < message.size(); j++) {
		// 	std::cerr << "\n" << j << ": " << message[j];
		// }
		// std::cerr << "\n";
		// std::cerr << "\n===\n";

		if (message.find('$') != std::string::npos) {
			rcv.terminate();
			std::cerr << "Hello\n";
			input_end = true;
		}
	}
	return 0;
}
