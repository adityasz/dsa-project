#include <string>
#include <cassert>
#include <unordered_map>

#include "receiver.h"
#include "part1.cpp"
#include "part2.cpp"
#include "part3.cpp"

int main(int argc, char *argv[])
{
	Receiver rcv;
	// FIXME
	// sleep(5);

	if (argc != 2)
		return 0xdead;

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

	return 0;
}
