#------------------------
# Do not modify this file
#------------------------

all: trader

# For part1: marked with a blank trader
market: main.cpp market.cpp worker.cpp blank_trader.cpp
	g++ -std=c++20 main.cpp market.cpp worker.cpp blank_trader.cpp -lpthread -o market.o -DDEBUG
	./market.o 2 > output.txt
	cat output.txt

# For part2
trader: main.cpp market.cpp worker.cpp trader.cpp
	g++ -std=c++20 main.cpp market.cpp worker.cpp trader.cpp -lpthread -o trader.o -DDEBUG
	./trader.o 2 > output.txt
	cat output.txt
debug: main.cpp market.cpp worker.cpp trader.cpp
	g++ -std=c++20 main.cpp market.cpp worker.cpp trader.cpp -lpthread -o trader.o -DDEBUG -g
	gdb --args trader.o 2

clean:
	rm -rf market.o trader.o output.txt
	rm -rf *~
