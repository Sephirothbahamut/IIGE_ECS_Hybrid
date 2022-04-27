#pragma once

#include <utils/wrapper.h>
#include <utils/string.h>
#include "../entt.h"

namespace iige::ecs
	{
	template <typename T, utils::compile_time::template_string name, bool has_in_place_delete = false>
	struct component : utils::wrapper<T>
		{
		using utils::wrapper<T>::wrapper;
		};
	template <typename T, utils::compile_time::template_string name>
	struct component<T, name, true> : utils::wrapper<T>
		{
		using utils::wrapper<T>::wrapper;
		static constexpr auto in_place_delete = true;
		};
	}