#include <string>
#include <vector>
#include <unordered_map>

#include <chrono>

#include "receiver.h"
#include "part1.cpp"
#include "part2.cpp"
#include "part3.cpp"

int main(int argc, char *argv[])
{
	Receiver rcv;
	// FIXME: doesn't work with sleep(5)
	// sleep(5);

	if (argc != 2)
		return 0xdead;

	auto start = std::chrono::steady_clock::now();
	switch (argv[1][0]) {
	case '1':
		part_1(rcv);
		break;
	case '2':
		part_2(rcv);
		break;
	case '3':
		part_3(rcv);
		break;
	}
	auto end = std::chrono::steady_clock::now();

	auto 🛺 = end - start;
	std::cerr << "Execution time: "
	          << std::chrono::duration<double, std::milli> (🛺).count()
	          << " ms\n";

	return 0;
}
