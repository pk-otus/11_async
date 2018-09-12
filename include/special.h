#pragma once
#include <memory>

#include "commands_block.h"
namespace async
{
	class special_command_handler
	{
	public:
		special_command_handler(size_t num_commands) :
			sz_fixed_buffer(num_commands),
			count_brackets(0) { }

		bool TryHandleSpecial(const std::string& cmd)
		{
			if ("{" == cmd)
			{
				if (0 == count_brackets) Flush();
				++count_brackets;
				return true;
			}
			if ("}" == cmd)
			{
				if (count_brackets)
				{
					--count_brackets;
					if (0 == count_brackets) Flush();
				}
				return true;
			}
			return false;
		}

		std::unique_ptr<commands_block> CreateCommandBlock() const
		{
			return std::unique_ptr<commands_block>
				(count_brackets
					? new commands_block()
					: new limited_commands_block(sz_fixed_buffer));
		}

	private:
		virtual void Flush() = 0;

		const size_t	sz_fixed_buffer;
		size_t			count_brackets;
	};
}