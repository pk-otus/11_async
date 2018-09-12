#include <iostream>
#include <chrono>

#include "data_reader.h"

int main(int argc, char const *argv[])
{
	try
	{
		int num_commands = 3;
		int num_threads = 2;

		if (argc >= 3)
		{
			num_threads = atoi(argv[2]);
		}
		if (argc >= 2)
		{
			num_commands = atoi(argv[1]);
			
		}
		else
		{
			std::cout <<	"Usage:\nbulk [commands = 3] [threads = 2]\nParameters: \n" <<
							"<commands>: number of packed commands\n" <<
							"<threads>: number of used threads\n";
			//return 1;
		}

		auto t_begin = std::chrono::high_resolution_clock::now();
		{
			async::data_reader reader(num_commands, num_threads);
			auto result = reader.Perform(std::cin);
			std::cout << result;
		}
		auto t_end = std::chrono::high_resolution_clock::now();
		auto time_span = std::chrono::duration <uint64_t, std::nano>(t_end - t_begin).count() / 1000000;
		std::cout << "\nDuration: " << time_span;
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
