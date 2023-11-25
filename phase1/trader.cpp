#include <string>
#include <vector>
#include <unordered_map>

#ifdef DEBUG
#  include <chrono>
#  define DEBUG_MSG(x) std::cerr << x;
#  define __get_time(x) auto x = std::chrono::steady_clock::now()
#  define __duration(t, x, y) auto t = std::chrono::duration<double, std::milli> (y - x).count()
#else
#  define DEBUG_MSG(x)
#  define __get_time(x)
#  define __duration(t, x, y)
#endif

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

	__get_time(start);
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
	__get_time(end);

	__duration(🛺, start, end);
	DEBUG_MSG("Execution time: " << 🛺 << " ms\n");

	return 0;
}
