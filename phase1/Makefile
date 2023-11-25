#------------------------
# Do not modify this file
#------------------------

all: traders socket

traders: trader.cpp receiver.h
	g++ -std=c++20 trader.cpp -o trader -DDEBUG

socket: iml.cpp
	g++ -std=c++20 iml.cpp -o iml

1: socket traders
	./trader 1 > output.txt &
	sleep 1
	./iml inputs/data1.txt
	@# tr '\r' '\n' < output.txt > temp_output.txt
	@#mv temp_output.txt output.txt
	@code --diff outputs/out1.txt output.txt

test1: socket traders
	python script.py 1
	./trader 1 > output.txt &
	sleep 1
	./iml input.txt
	@# tr '\r' '\n' < output.txt > temp_output.txt
	@#mv temp_output.txt output.txt
	@cat output.txt

2: socket traders
	./trader 2 > output.txt &
	sleep 1
	./iml inputs/data2.txt
	@# tr '\r' '\n' < output.txt > temp_output.txt
	@#mv temp_output.txt output.txt
	@code --diff outputs/out2.txt output.txt

test2: socket traders
	python script.py 2
	./trader 2 > output.txt &
	sleep 1
	./iml input.txt
	@# tr '\r' '\n' < output.txt > temp_output.txt
	@#mv temp_output.txt output.txt
	@cat output.txt

3: socket traders
	./trader 3 > output.txt &
	sleep 1
	./iml inputs/data3.txt
	@#tr '\r' '\n' < output.txt > temp_output.txt
	@#mv temp_output.txt output.txt
	@code --diff outputs/out3.txt output.txt

test3: socket traders
	python script.py 3
	./trader 3 > output.txt &
	sleep 1
	./iml input.txt
	@# tr '\r' '\n' < output.txt > temp_output.txt
	@#mv temp_output.txt output.txt
	@cat output.txt

clean:
	rm -rf output.txt trader iml
	rm -rf *~
