#include <algorithm>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

#include "..//include/async.h"
#include "..//include/data_reader.h"

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