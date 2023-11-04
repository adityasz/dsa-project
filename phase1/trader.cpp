#include <string>
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
	// int i = 0;
	std::string sp = "";
	while (!input_end) {
		// i++;
		std::string message = rcv.readIML();
		solution(message, sp);
		// std::cout << i << "\n" << message;
		if (message.find('$') != std::string::npos) {
			rcv.terminate();
			input_end = true;
		}
	}
	return 0;
}
