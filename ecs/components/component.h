#pragma once

#include <utils/wrapper.h>

namespace iige::ecs
	{
	template <typename T>
	struct component : utils::wrapper<T>
		{
		using utils::wrapper<T>::wrapper;
		};

	template <typename T>
	struct component_in_place_delete : component<T>
		{
		using component<T>::component;
		static constexpr auto in_place_delete = true;
		};
	}