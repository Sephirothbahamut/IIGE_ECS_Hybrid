#pragma once

#include <utils/wrapper.h>
#include "../entt.h"

namespace iige::ecs
	{
	template <typename T, entt::id_type index, bool has_in_place_delete = false>
	struct component : utils::wrapper<T>
		{
		inline static constexpr entt::id_type index{index};
		using utils::wrapper<T>::wrapper;
		};
	template <typename T, entt::id_type index>
	struct component<T, index, true> : utils::wrapper<T>
		{
		inline static constexpr entt::id_type index{index};
		using utils::wrapper<T>::wrapper;
		static constexpr auto in_place_delete = true;
		};
	}