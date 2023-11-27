#ifndef MARKET_H
#define MARKET_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

class market {
public:
	market(int argc, char** argv);
	void start();
	std::vector<std::string> lines;
};

#endif
