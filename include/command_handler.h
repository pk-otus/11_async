#pragma once
#include <memory>
#include <ctime>

#include "commands_block.h"

namespace async
{
	class command_handler
	{
	public:
		command_handler(size_t instance_id, size_t num_commands) :
			instance_suffix		(instance_id),
			sz_fixed_buffer		(num_commands),			
			count_brackets		(0),
			block_suffix		(0),
			commands			(nullptr)
		{ }

		~command_handler()
		{
			if (commands)
				commands->final_flush();
		}

		void handle_command(const std::string& cmd)
		{
			if (1 == cmd.size() && try_handle_special(cmd[0]))
				return;

			if (!commands)
				commands = create_command_block();

			commands->add_command(cmd);
			if (commands->is_full())
				flush();
		}		

	private:
		std::unique_ptr<commands_block> create_command_block()
		{
			auto fname = "bulk" + std::to_string(time(nullptr)) +
				'_' + std::to_string(instance_suffix) +
				'_' + std::to_string(block_suffix++) + ".log";

			return std::unique_ptr<commands_block>
				(count_brackets
					? new commands_block(fname)
					: new limited_commands_block(fname, sz_fixed_buffer));
		}

		bool try_handle_special(char ch)
		{
			if ('{' == ch)
			{
				if (0 == count_brackets) flush();
				++count_brackets;
				return true;
			}
			if ('}' == ch)
			{
				if (count_brackets)
				{
					--count_brackets;
					if (0 == count_brackets) flush();
				}
				return true;
			}
			return false;
		}

		void flush()
		{
			if (commands)
			{
				commands->log_all();
				commands = nullptr;
			}
		}

		const size_t	instance_suffix;
		const size_t	sz_fixed_buffer;
		
		size_t			count_brackets;
		size_t			block_suffix;

		std::unique_ptr<commands_block>	commands;
	};
}
