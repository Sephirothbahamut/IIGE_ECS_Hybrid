#pragma once

#include <limits>

template <typename T>
constexpr size_t get_id() noexcept { return std::numeric_limits<size_t>::max(); };


class multitype_container
	{
	public:
		template <typename T>
		void emplace()
			{
			
			}
	private:
	};
