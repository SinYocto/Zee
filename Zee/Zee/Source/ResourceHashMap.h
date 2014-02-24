#ifndef RESOURCE_HASH_MAP_H
#define RESOURCE_HASH_MAP_H

#include <functional>
#include <hash_map>

struct WCharLess : public std::binary_function<const wchar_t*, const wchar_t*, bool>
{
public:
	result_type operator()(const first_argument_type& left, const second_argument_type& right) const
	{
		return (_wcsicmp(left, right) < 0 ? true : false);
	}
};

#endif