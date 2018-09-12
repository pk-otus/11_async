#pragma once
#include <iostream>
#include <queue>
#include <thread>
#include <map>
#include <condition_variable>
#include <algorithm>

#include "statistics.h"

template<typename T> T Pop(std::queue<T>& q)
{
	auto ptr = std::move(q.front());
	q.pop();
	return std::move(ptr);
}
namespace async
{
	using cout_element_t = std::pair<size_t, std::string>;
	using file_element_t = std::unique_ptr<commands_block>;

	template <typename T>
	class task_queue
	{
	public:
		explicit task_queue(size_t num_threads) :
			isWorking(true)
		{
			for (size_t i = 0; i < num_threads; ++i)
			{
				threads.emplace_back(&task_queue::DoWork, this);
				st.insert({ threads.back().get_id(),
							stat_counter(1 == num_threads ? 0 : threads.size()) });
			}
		}

		virtual ~task_queue()
		{
			JoinAll();
		}

		std::vector<stat_counter> GetStatistics()
		{
			JoinAll();

			std::vector<stat_counter> result;
			transform(st.begin(), st.end(), std::back_inserter(result),
				[](const auto& d) { return d.second; });

			return result;
		}
		void Push(std::unique_ptr<commands_block>& elem)
		{
			std::lock_guard<std::mutex> lock(guard_mutex);
			Enqueue(elem);
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

		void DoWork() // active object pattern
		{
			while (isWorking)
			{
				std::unique_lock<std::mutex> lock(guard_mutex);
				while (queue_elements.empty() && isWorking)
				{  // loop to avoid spurious wakeups
					cond_var.wait(lock);
				}

				if (isWorking)
					PopQueue();
			}
			{
				std::unique_lock<std::mutex> lock(guard_mutex);
				while (!queue_elements.empty())
					PopQueue();
			}

		}

		void Enqueue(std::unique_ptr<commands_block>& elem);
		void PopQueue();

		bool									isWorking;
		std::mutex								guard_mutex;
		std::condition_variable					cond_var;
		std::vector<std::thread>				threads;
		std::map<std::thread::id, stat_counter> st;
		std::queue<element_t>					queue_elements;
	};

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
	inline void task_queue<file_element_t>::PopQueue()
	{
		auto e = Pop(queue_elements);
		auto& stat = st.at(std::this_thread::get_id());
		stat.AddBlock(e->CommandsCount());
		e->LogToFile(stat.thread_number, stat.block_counter);
	}

	template <>
	inline void task_queue<cout_element_t>::PopQueue()
	{
		auto e = Pop(queue_elements);
		st.at(std::this_thread::get_id()).AddBlock(e.first);
		std::cout << e.second << std::endl;
	}
}