// This is your trader. Place your orders from here

#include <string>
#include <iostream>
#include <atomic>

extern std::atomic<int> commonTimer;

int reader(int time)
{
	std::cout << time << "Harry SELL AMD $1 #32 1" << std::endl;
	return 1;
}

int trader(std::string *message)
{
	return 1;
}

void* userThread(void *arg)
{
	int thread_id = *(int *) arg;
	while (true) {
		int currentTime;
		{
			currentTime = commonTimer.load();
		}
		int end = reader(currentTime);
		if (end)
			break;
	}
	return nullptr;
}

void* userTrader(void *arg)
{
	return nullptr;
}
