#pragma once
#include <iostream>
#include <queue>
#include <thread>
#include <condition_variable>
#include <algorithm>

#include "statistics.h"
#include <atomic>

namespace async
{
	template <typename T>
	class task_queue
	{
	public:
		explicit task_queue(size_t num_threads) :
			isWorking(true)
		{
			for (size_t i = 0; i < num_threads; ++i)
			{
				threads.emplace_back(
							&task_queue::DoWork, 
							this, 
							1 == num_threads ? 0 : threads.size());
			}
		}

		virtual ~task_queue()
		{
			JoinAll();
		}

		std::vector<stat_counter> GetStatistics()
		{
			JoinAll();
			return stats;
		}
		void Push(std::unique_ptr<commands_block>& elem)
		{
			{
				std::lock_guard<std::mutex> lock(guard_mutex);
				Enqueue(elem);
			}
			cond_var.notify_one();
		}

	private:
		using element_t = T;

		void JoinAll()
		{
			isWorking = false;
			cond_var.notify_all();

			for (auto& thrd : threads)
			{
				if (thrd.joinable())
					thrd.join();
			}
		}

		void DoWork(size_t num) // active object pattern
		{
			stat_counter stat(num);

			while (isWorking)
			{			
				std::unique_lock<std::mutex> lock(guard_mutex);
				cond_var.wait(lock, 
							[this]() { return !queue_elements.empty() || !isWorking; });

				if (isWorking)
				{
					auto e = std::move(queue_elements.front());
					queue_elements.pop();
					lock.unlock();
					stat.AddBlock(CommandsCount(e));
					Log(e, stat);
				}
			}
			{
				std::unique_lock<std::mutex> lock(guard_mutex);
				while (!queue_elements.empty())
				{
					auto e = std::move(queue_elements.front());
					queue_elements.pop();
					stat.AddBlock(CommandsCount(e));
					Log(e, stat);
				}
				stats.push_back(stat);
			}
		}

		void Enqueue(std::unique_ptr<commands_block>& elem);

		static size_t CommandsCount(const element_t& e);
		static void Log(const element_t& e, const stat_counter& st);

		std::atomic<bool>			isWorking;
		std::mutex					guard_mutex;
		std::condition_variable		cond_var;
		std::vector<std::thread>	threads;
		std::vector<stat_counter>	stats;
		std::queue<element_t>		queue_elements;
	};

	using cout_element_t = std::pair<size_t, std::string>;
	using file_element_t = std::unique_ptr<commands_block>;

	template <>
	inline void task_queue<file_element_t>::Enqueue(std::unique_ptr<commands_block>& elem)
	{
		queue_elements.emplace(std::move(elem));
	}
	template <>
	inline void task_queue<cout_element_t>::Enqueue(std::unique_ptr<commands_block>& elem)
	{
		queue_elements.emplace(elem->CommandsCount(), elem->GetString());
	}

	template <>
	inline size_t task_queue<file_element_t>::CommandsCount(const element_t& e)
	{		
		return e->CommandsCount();
	}

	template <>
	inline size_t task_queue<cout_element_t>::CommandsCount(const element_t& e)
	{
		return e.first;
	}

	template <>
	inline void task_queue<file_element_t>::Log(const element_t& e, const stat_counter& st)
	{
		e->LogToFile(st.thread_number, st.block_counter);
	}
	template <>
	inline void task_queue<cout_element_t>::Log(const element_t& e, const stat_counter& st) 
	{
		std::cout << e.second << std::endl;
	}
}