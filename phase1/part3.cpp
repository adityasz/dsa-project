void part_3_sol();
void part_3(Receiver rcv)
{
	bool input_end = false;
	std::string sp = "";

	// Initialize to 64 to prevent frequent resize && copy operations
	std::vector<std::unordered_map<std::string, long long>> lin_combs(64);
	std::vector<std::pair<long long, char>>                 prices(64);
	int num_alive = 0;

	while (!input_end) {
		std::string message = rcv.readIML();
		// part_3_sol(message, sp, num_alive, lin_combs, prices);

		if (message.find('$') != std::string::npos) {
			rcv.terminate();
			DEBUG_MSG("───────────────────────────────────────────────────\n");
			input_end = true;
		}
	}
}
