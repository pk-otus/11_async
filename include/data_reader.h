#pragma once
#include <queue>
#include <condition_variable>
#include <atomic>
#include <thread>

#include "command_handler.h"

namespace async
{
	class data_reader 
	{
	public:
		data_reader(size_t instance_id, size_t num_commands) :
			handler		(instance_id, num_commands),			
			isWorking	(true)
		{
			thread_consumer = std::thread(&data_reader::consumer_worker, this);
		}

		~data_reader()
		{
			isWorking = false;
			cond_var.notify_all();						
			thread_consumer.join();
		}
		
		void receive(const std::string& str)
		{	
			bool needs_notify = false;

			{
				std::lock_guard<std::mutex> lock(guard_mutex);
				buffer += str;

				auto pos_end = buffer.find('\n');
				while (std::string::npos != pos_end)
				{
					auto cmd = buffer.substr(0, pos_end);
					buffer = buffer.substr(pos_end + 1);
					pos_end = buffer.find('\n');

					if (!cmd.empty())
					{
						q.push(cmd);
						needs_notify = true;
					}
				}
			}
			if (needs_notify)
				cond_var.notify_one();
		}
			
	private:		
		void consumer_worker()
		{
			while (isWorking)
			{
				std::unique_lock<std::mutex> lock(guard_mutex);
				cond_var.wait(lock,
					[this]() { return !q.empty() || !isWorking; });

				if (q.empty())
					continue;

				auto cmd = q.front();
				q.pop();
				lock.unlock();
				handler.handle_command(cmd);

			}
			while (!q.empty()) 
			{
				handler.handle_command(q.front());
				q.pop();
			}
		}
				
		std::string					buffer;
		std::queue<std::string>		q;
		command_handler				handler;

		std::atomic<bool>			isWorking;
		std::mutex					guard_mutex;
		std::condition_variable		cond_var;
				
		std::thread					thread_consumer;		
	};
}
