#pragma once
#include <cstddef>

#ifdef _WIN32
#	ifdef ASYNC_EXPORTS
#		define ASYNC_API __declspec(dllexport)
#	else
#		define ASYNC_API __declspec(dllimport)
#	endif
#else
#	define ASYNC_API
#endif

namespace async 
{
	using handle_t = void *;

	ASYNC_API handle_t	connect		(std::size_t bulk);
	ASYNC_API void		receive		(handle_t handle, const char *data, std::size_t size);
	ASYNC_API void		disconnect	(handle_t handle);

}
