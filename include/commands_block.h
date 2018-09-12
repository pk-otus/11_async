#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <ctime>
#include <iterator>
namespace async
{
	class commands_block
	{
	public:
		commands_block() : tm_created(time(nullptr)) { }

		commands_block(const commands_block&) = delete;
		commands_block& operator=	(const commands_block&) = delete;

		virtual ~commands_block() = default;

		void LogToFile(size_t thread_number, size_t cnt) const
		{
			auto name_file = "bulk" + std::to_string(tm_created)
				+ '_' + std::to_string(thread_number)
				+ '_' + std::to_string(cnt) + ".log";

			auto stream = std::ofstream(name_file, std::ofstream::out);
			stream << GetString();
			stream.close();
		}

		std::string GetString() const
		{
			std::string result;
			if (!pool_commands.empty())
			{
				const std::string DELIMITER = ", ";
				result += "bulk: ";
				for (const auto& item : pool_commands)
				{
					result += item + DELIMITER;
				}
				result = result.substr(0, result.size() - DELIMITER.size());
			}
			return result;
		}

		virtual bool IsFull() const { return false; }

		void AddCommand(const std::string& cmd)
		{
			pool_commands.emplace_back(cmd);
		}

		size_t CommandsCount() const { return pool_commands.size(); }

	private:
		const time_t				tm_created;
		std::vector<std::string>	pool_commands;
	};

	class limited_commands_block : public commands_block
	{
	public:
		explicit limited_commands_block(size_t sz) : sz_fixed_buffer(sz) { }

		bool IsFull() const override
		{
			return sz_fixed_buffer == CommandsCount();
		}

	private:
		const size_t				sz_fixed_buffer;
	};
}