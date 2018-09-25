#pragma once
#include <algorithm>

#include "special.h"
#include "task_queue.h"

namespace async
{
	class data_reader : special_command_handler
	{
	public:
		data_reader(size_t num_commands, size_t cnt_file_threads) :
			special_command_handler(num_commands),
			file_threads(cnt_file_threads),
			cout_threads(1),
			commands(nullptr) {}

		~data_reader() = default;

		data_reader_results Perform(std::istream& input_stream)
		{
			for (std::string strCmd; std::getline(input_stream, strCmd);)
			{
				strCmd.erase(std::remove(strCmd.begin(), strCmd.end(), '\r'), strCmd.end());
				if (strCmd.empty()) continue;
				
				if (TryHandleSpecial(strCmd))
				{
					stats.AddString();
					continue;
				}
				if (!commands)
				{
					commands = CreateCommandBlock();
				}

				commands->AddCommand(strCmd);
				if (commands->IsFull())
					Flush();
			}
			if (dynamic_cast<limited_commands_block*>(commands.get()))
			{
				Flush();
			}
			else
			{
				if (commands)
					stats.AddString(commands->CommandsCount());
			}

			return { stats,
						cout_threads.GetStatistics().front(),
						file_threads.GetStatistics() };
		}
	private:
		void Flush() override
		{
			if (commands)
			{
				stats.basic_stat.AddBlock(commands->CommandsCount());
				cout_threads.Push(commands);
				file_threads.Push(commands);

				commands = nullptr;
			}
		}

		task_queue<file_element_t> file_threads;
		task_queue<cout_element_t> cout_threads;

		stat_special_counter stats;
		std::unique_ptr<commands_block>	commands;
	};
}