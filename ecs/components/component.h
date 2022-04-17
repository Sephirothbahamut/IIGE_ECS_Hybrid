#pragma once

#include <utils/wrapper.h>

namespace iige::ecs
	{
	template <typename T, size_t index = 0, bool has_in_place_delete = false>
	struct component : utils::wrapper<T>
		{
		using utils::wrapper<T>::wrapper;
		};
	template <typename T, size_t index>
	struct component<T, index, true> : utils::wrapper<T>
		{
		using utils::wrapper<T>::wrapper;
		static constexpr auto in_place_delete = true;
		};
	}