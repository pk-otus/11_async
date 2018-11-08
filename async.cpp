#define ASYNC_EXPORTS

#include "../include/async.h"
#include "../include/data_reader.h"

namespace async
{
	handle_t connect(std::size_t bulk)
	{
		static size_t instance_count = 0;
		return new data_reader(instance_count++, bulk);
	}

	void receive(handle_t handle, const char *data, std::size_t size)
	{
		auto h = static_cast<data_reader*>(handle);
		h->receive(std::string(data, size));

	}
	void disconnect(handle_t handle)
	{
		delete static_cast<data_reader*>(handle);
	}
}