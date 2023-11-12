#------------------------
# Do not modify this file
#------------------------

all: trader iml

traders: trader.cpp receiver.h
	g++ -std=c++20 trader.cpp -o trader

socket: iml.cpp
	g++ -std=c++20 iml.cpp -o iml

1: iml trader
	./trader 1 > output.txt &
	sleep 1
	./iml inputs/data1.txt
	@# tr '\r' '\n' < output.txt > temp_output.txt
	@#mv temp_output.txt output.txt
	@code --diff outputs/out1.txt output.txt

test: iml trader
	./trader 1 > output.txt &
	sleep 1
	./iml input.txt
	@# tr '\r' '\n' < output.txt > temp_output.txt
	@#mv temp_output.txt output.txt
	@code --diff outputs/out1.txt output.txt

2: iml trader
	./trader 2 > output.txt &
	sleep 1
	./iml inputs/data2.txt
	@# tr '\r' '\n' < output.txt > temp_output.txt
	@#mv temp_output.txt output.txt
	@code --diff outputs/out2.txt output.txt

3: iml trader
	./trader 3 > output.txt &
	sleep 1
	./iml inputs/data3.txt
	@#tr '\r' '\n' < output.txt > temp_output.txt
	@#mv temp_output.txt output.txt
	@code --diff outputs/out3.txt output.txt

clean:
	rm -rf output.txt trader iml
	rm -rf *~
