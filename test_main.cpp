#include <algorithm>
#include <fstream>
#include <string>
#include <chrono>
#include <future>
#include <sstream>

#include <gtest/gtest.h>

#include "include/data_reader.h"
#include "include/async.h"

using test_result_t = std::string;

test_result_t GetTestResult(size_t test_number, int test_bulk_size)
{
	testing::internal::CaptureStdout();
	{
		std::ifstream file_in("in" + std::to_string(test_number) + ".txt", std::ofstream::in);

		std::string str_file(	(std::istreambuf_iterator<char>(file_in)),
								std::istreambuf_iterator<char>());
		str_file.erase(std::remove(str_file.begin(), str_file.end(), '\r'), str_file.end());		
		file_in.close();

		auto reader = async::connect(test_bulk_size);

		for (int pos = 0; pos < str_file.size(); pos += 3)
		{
			async::receive(reader, str_file.c_str() + pos, 3);
		}

		async::disconnect(reader);
	}
	std::string result = testing::internal::GetCapturedStdout();
	return result;
}

std::string GetControlResult(size_t test_number)
{
	std::string strOutFile = "out" + std::to_string(test_number) + ".txt";

	std::ifstream t(strOutFile);
	std::string result((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
	result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
	return result;
}


TEST(AsyncTest, test1)
{
	std::string control = GetControlResult(1);
	auto tested = GetTestResult(1, 3);
	ASSERT_EQ(tested, control);
}

TEST(AsyncTest, test2)
{
	std::string control = GetControlResult(2);
	auto tested = GetTestResult(2, 3);
	ASSERT_EQ(tested, control);
}

TEST(AsyncTest, test3)
{
	std::string control = GetControlResult(3);
	auto tested = GetTestResult(3, 3);
	ASSERT_EQ(tested, control);
}

TEST(AsyncTest, test4)
{
	std::string control = GetControlResult(4);
	auto tested = GetTestResult(4, 3);
	ASSERT_EQ(tested, control);
}

TEST(AsyncTest, test5)
{
	std::string control = GetControlResult(5);
	auto tested = GetTestResult(5, 2);
	ASSERT_EQ(tested, control);
}

TEST(AsyncTest, separated)
{
	std::string control = "bulk: cmd1, cmd2, cmd3\nbulk: cmd4\n";

	testing::internal::CaptureStdout();
	auto reader = async::connect(3);
	async::receive(reader, "cm", 2);
	async::receive(reader, "d1", 2);
	async::receive(reader, "\nc", 2);
	async::receive(reader, "md", 2);
	async::receive(reader, "2\n", 2);
	async::receive(reader, "cm", 2);
	async::receive(reader, "d3", 2);
	async::receive(reader, "\nc", 2);
	async::receive(reader, "md", 2);
	async::receive(reader, "4\n", 2);

	async::disconnect(reader);
	std::string result = testing::internal::GetCapturedStdout();

	ASSERT_EQ(result, control);
}

static const int N_COMMANDS = 10000;

void run1()
{
	std::size_t bulkSize = 2;
	auto h1 = async::connect(bulkSize);
	for (int i = 0; i < N_COMMANDS; ++i)
	{
		std::stringstream ss;
		ss << i << " h1 bulkSize=2";
		if (i < N_COMMANDS - 1)
			ss << std::endl;
		auto str = ss.str();
		async::receive(h1, str.c_str(), str.length());
	}
	async::disconnect(h1);
	std::this_thread::sleep_for(std::chrono::seconds(2));
}

void run2()
{
	std::size_t bulkSize = 1;
	auto h2 = async::connect(bulkSize);
	for (int i = 0; i < N_COMMANDS; ++i)
	{
		std::stringstream ss;
		ss << i << " h2 bulkSize=1";
		if (i < N_COMMANDS - 1)
			ss << std::endl;
		auto str = ss.str();
		async::receive(h2, str.c_str(), str.length());
	}
	async::disconnect(h2);
	std::this_thread::sleep_for(std::chrono::seconds(2));
}

void run3()
{
	std::size_t bulkSize = 3;
	auto h3 = async::connect(bulkSize);
	for (int i = 0; i < N_COMMANDS; ++i)
	{
		std::stringstream ss;
		ss << i << " h3 bulkSize=3";
		if (i < N_COMMANDS - 1)
			ss << std::endl;
		auto str = ss.str();
		async::receive(h3, str.c_str(), str.length());
	}
	async::disconnect(h3);
	std::this_thread::sleep_for(std::chrono::seconds(2));
}

TEST(AsyncTest, mike)
{
	auto result1 = std::async(std::launch::async, run1);
	auto result2 = std::async(std::launch::async, run2);
	auto result3 = std::async(std::launch::async, run3);
	std::size_t bulk = 5;
	auto h4 = async::connect(bulk);
	auto h5 = async::connect(bulk);
	for (int i = 0; i < N_COMMANDS; ++i)
	{
		{
			std::stringstream ss;
			ss << i << " h4 bulkSize=5";
			if (i < N_COMMANDS - 1)
				ss << std::endl;
			auto str = ss.str();
			async::receive(h4, str.c_str(), str.length());
		}
		{
			std::stringstream ss;
			ss << i << " h5 bulkSize=5";
			if (i < N_COMMANDS - 1)
				ss << std::endl;
			auto str = ss.str();
			async::receive(h5, str.c_str(), str.length());
		}
	}
	async::disconnect(h4);
	async::disconnect(h5);
	std::this_thread::sleep_for(std::chrono::seconds(2));
	result1.wait();
	result2.wait();
	result3.wait();
}