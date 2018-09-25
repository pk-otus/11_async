#pragma once
#include <sstream>
namespace async
{
	struct stat_counter
	{
		explicit stat_counter(size_t num) : thread_number(num) {}

		void AddBlock(size_t countCommands)
		{
			++block_counter;
			command_counter += countCommands;
		}

		friend std::ostream& operator<<(std::ostream& os, const stat_counter& obj)
		{
			os << obj.block_counter << " blocks, " <<
				obj.command_counter << " commands";
			return os;
		}
		size_t thread_number;
		size_t block_counter = 0;
		size_t command_counter = 0;
	};

	struct stat_special_counter
	{
		void AddString(size_t cnt = 1)
		{
			string_counter += cnt;
		}

		friend std::ostream& operator<<(std::ostream& os, const stat_special_counter& obj)
		{
			os << (obj.string_counter + obj.basic_stat.command_counter)
				<< " strings, " << obj.basic_stat;
			return os;
		}

		stat_counter		basic_stat{ 0 };
		size_t				string_counter = 0;
	};

	struct data_reader_results
	{
		bool IsValidResults()
		{
			bool is_valid_cout =
				(stat_main.basic_stat.command_counter == stat_cout.command_counter) &&
				(stat_main.basic_stat.block_counter == stat_cout.block_counter);

			size_t cmds = 0, blocks = 0;
			for (auto& f_stat : stat_file)
			{
				cmds += f_stat.command_counter;
				blocks += f_stat.block_counter;
			}
			bool is_valid_file =
				(cmds == stat_cout.command_counter) &&
				(blocks == stat_cout.block_counter);


			if (!is_valid_file)
			{
				std::ofstream fs = std::ofstream("error.txt", std::ios_base::out);
				fs << *this;
			}
			return is_valid_cout && is_valid_file;
		}

		friend std::ostream& operator<<(std::ostream& os, const data_reader_results& obj)
		{
			os << "main: " << obj.stat_main << '\n'
				<< "log: " << obj.stat_cout << '\n';
			int i = 0;
			for (auto& f_stat : obj.stat_file)
			{
				os << "file" + std::to_string(++i) + ": " << f_stat << '\n';
			}
			return os;
		}

		stat_special_counter stat_main;
		stat_counter stat_cout;
		std::vector<stat_counter> stat_file;
	};
}