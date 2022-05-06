#pragma once

#include <utils/math/geometry/transformations.h>
#include <utils/math/math.h>
#include <utils/definitions.h>
#include <utils/string.h>

#include "../../types/core.h"

#include "collision.h"
#include "component.h"

namespace iige::ecs::components::transform
	{
	//namespace details
	//	{
	//	template <typename T, utils::compile_time::template_string name>
	//	struct element : component<T, name>
	//		{
	//		using component<T, name>::component;
	//		using min = component<T, utils::compile_time::concat<name, "_min">()>;
	//		using max = component<T, utils::compile_time::concat<name, "_max">()>;
	//		};
	//	
	//	template <utils::compile_time::template_string name>
	//	struct has_translation_magnitude
	//		{
	//		struct translation_magnitude
	//			{
	//			using min = component<float, utils::compile_time::concat<name, "_translation_magnitude_min">()>;
	//			using max = component<float, utils::compile_time::concat<name, "_translation_magnitude_max">()>;
	//			};
	//		};
	//
	//	template <utils::compile_time::template_string name>
	//	struct has_next
	//		{
	//		struct next
	//			{
	//			using x     = component<float     , utils::compile_time::concat<name, "_next_x"    >()>;
	//			using y     = component<float     , utils::compile_time::concat<name, "_next_y"    >()>;
	//			using angle = component<angle::rad, utils::compile_time::concat<name, "_next_angle">()>;
	//			using scale = component<float     , utils::compile_time::concat<name, "_next_scale">()>;
	//			};
	//		};
	//
	//	template <utils::compile_time::template_string name, template<utils::compile_time::template_string> class ...Ts>
	//	struct base : Ts<name>... 
	//		{
	//		using x     = element<float     , utils::compile_time::concat<name, "_x"        >()>;
	//		using y     = element<float     , utils::compile_time::concat<name, "_y"        >()>;
	//		using angle = element<angle::rad, utils::compile_time::concat<name, "_angle"    >()>;
	//		using scale = element<float     , utils::compile_time::concat<name, "_scale"    >()>;
	//		};
	//	}
	//
	//using relative     = details::base<"relative"    , details::has_next>;
	//using absolute     = details::base<"absolute"    , details::has_next>;
	//using interpolated = details::base<"interpolated">;
	//using speed        = details::base<"speed"       , details::has_translation_magnitude>;
	//using acceleration = details::base<"acceleration", details::has_translation_magnitude>;

	struct relative
		{
		struct x     : component<float, "relative_x"    > 
			{ 
			using component<float, "relative_x">::component;
			using min = component<float, "relative_x_min">; 
			using max = component<float, "relative_x_max">; 
			};
		struct y     : component<float, "relative_y"    > 
			{
			using component<float, "relative_y">::component;
			using min = component<float, "relative_y_min">;
			using max = component<float, "relative_y_max">; 
			};
		struct angle : component<iige::angle::rad, "relative_angle"> 
			{
			using component<iige::angle::rad, "relative_angle">::component;
			using min = component<iige::angle::rad, "relative_angle_min">;
			using max = component<iige::angle::rad, "relative_angle_max">;
			};
		struct scale : component<float, "relative_scale"> 
			{
			using component<float, "relative_scale">::component;
			using min = component<float, "relative_scale_min">; 
			using max = component<float, "relative_scale_max">; 
			};
		struct next
			{
			struct x     : component<float           , "relative_next_x"    > 
				{ 
				using      component<float           , "relative_next_x"    >::component;
				using current_t = relative::x;
				};
			struct y     : component<float           , "relative_next_y"    > 
				{ 
				using      component<float           , "relative_next_y"    >::component;
				using current_t = relative::y;
				};
			struct angle : component<iige::angle::rad, "relative_next_angle"> 
				{ 
				using      component<iige::angle::rad, "relative_next_angle">::component;
				using current_t = relative::angle;
				};
			struct scale : component<float           , "relative_next_scale"> 
				{ 
				using      component<float           , "relative_next_scale">::component;
				using current_t = relative::scale;
				};
			};
		};
	struct absolute
		{
		struct x : component<float, "absolute_x">
			{
			using component<float, "absolute_x">::component;
			using min = component<float, "absolute_x_min">;
			using max = component<float, "absolute_x_max">;
			};
		struct y : component<float, "absolute_y">
			{
			using component<float, "absolute_y">::component;
			using min = component<float, "absolute_y_min">;
			using max = component<float, "absolute_y_max">;
			};
		struct angle : component<iige::angle::rad, "absolute_angle">
			{
			using component<iige::angle::rad, "absolute_angle">::component;
			using min = component<iige::angle::rad, "absolute_angle_min">;
			using max = component<iige::angle::rad, "absolute_angle_max">;
			};
		struct scale : component<float, "absolute_scale">
			{
			using component<float, "absolute_scale">::component;
			using min = component<float, "absolute_scale_min">;
			using max = component<float, "absolute_scale_max">;
			};
		struct next
			{
			struct x     : component<float           , "absolute_next_x"    > 
				{ 
				using      component<float           , "absolute_next_x"    >::component;
				using current_t = absolute::x;
				};
			struct y     : component<float           , "absolute_next_y"    > 
				{ 
				using      component<float           , "absolute_next_y"    >::component;
				using current_t = absolute::y;
				};
			struct angle : component<iige::angle::rad, "absolute_next_angle"> 
				{ 
				using      component<iige::angle::rad, "absolute_next_angle">::component;
				using current_t = absolute::angle;
				};
			struct scale : component<float           , "absolute_next_scale"> 
				{ 
				using      component<float           , "absolute_next_scale">::component;
				using current_t = absolute::scale;
				};
			};
		};
	struct interpolated
		{
		using x     = component<float, "interpolated_x">;
		using y     = component<float, "interpolated_y">;
		using angle = component<iige::angle::rad, "interpolated_angle">;
		using scale = component<float, "interpolated_scale">;
		};
	struct speed
		{
		struct x : component<float, "speed_x">
			{
			using component<float, "speed_x">::component;
			using min = component<float, "speed_x_min">;
			using max = component<float, "speed_x_max">;
			};
		struct y : component<float, "speed_y">
			{
			using component<float, "speed_y">::component;
			using min = component<float, "speed_y_min">;
			using max = component<float, "speed_y_max">;
			};
		struct angle : component<iige::angle::rad, "speed_angle">
			{
			using component<iige::angle::rad, "speed_angle">::component;
			using min = component<iige::angle::rad, "speed_angle_min">;
			using max = component<iige::angle::rad, "speed_angle_max">;
			};
		struct scale : component<float, "speed_scale">
			{
			using component<float, "speed_scale">::component;
			using min = component<float, "speed_scale_min">;
			using max = component<float, "speed_scale_max">;
			};
		struct next
			{
			using x = component<float, "speed_next_x">;
			using y = component<float, "speed_next_y">;
			using angle = component<iige::angle::rad, "speed_next_angle">;
			using scale = component<float, "speed_next_scale">;
			};
		struct translation_magnitude
			{
			using min = component<float, "speed_translation_magnitude_min">;
			using max = component<float, "speed_translation_magnitude_max">;
			};
		};
	struct acceleration
		{
		struct x : component<float, "acceleration_x">
			{
			using component<float, "acceleration_x">::component;
			using min = component<float, "acceleration_x_min">;
			using max = component<float, "acceleration_x_max">;
			};
		struct y : component<float, "acceleration_y">
			{
			using component<float, "acceleration_y">::component;
			using min = component<float, "acceleration_y_min">;
			using max = component<float, "acceleration_y_max">;
			};
		struct angle : component<iige::angle::rad, "acceleration_angle">
			{
			using component<iige::angle::rad, "acceleration_angle">::component;
			using min = component<iige::angle::rad, "acceleration_angle_min">;
			using max = component<iige::angle::rad, "acceleration_angle_max">;
			};
		struct scale : component<float, "acceleration_scale">
			{
			using component<float, "acceleration_scale">::component;
			using min = component<float, "acceleration_scale_min">;
			using max = component<float, "acceleration_scale_max">;
			};
		struct next
			{
			using x = component<float, "acceleration_next_x">;
			using y = component<float, "acceleration_next_y">;
			using angle = component<iige::angle::rad, "acceleration_next_angle">;
			using scale = component<float, "acceleration_next_scale">;
			};
		struct translation_magnitude
			{
			using min = component<float, "acceleration_translation_magnitude_min">;
			using max = component<float, "acceleration_translation_magnitude_max">;
			};
		};
	struct moved
		{
		static constexpr auto in_place_delete = true;
		};



	template <typename T>
	concept is_x = ::utils::concepts::any_of<T, absolute::x, absolute::next::x, relative::x, relative::next::x, speed::x, acceleration::x, interpolated::x>;
	template <typename T>
	concept is_y = ::utils::concepts::any_of<T, absolute::y, absolute::next::y, relative::y, relative::next::y, speed::y, acceleration::y, interpolated::y>;
	template <typename T>
	concept is_angle = ::utils::concepts::any_of<T, absolute::angle, absolute::next::angle, relative::angle, relative::next::angle, speed::angle, acceleration::angle, interpolated::angle>;
	template <typename T>
	concept is_scale = ::utils::concepts::any_of<T, absolute::scale, absolute::next::scale, relative::scale, relative::next::scale, speed::scale, acceleration::scale, interpolated::scale>;

	template <typename T>
	concept is_next = ::utils::concepts::any_of
		<T, 
		absolute::next::x, absolute::next::y, absolute::next::angle, absolute::next::scale, 
		relative::next::x, relative::next::y, relative::next::angle, relative::next::scale
		>;

	template <typename T>
	concept is_transform_component = is_x<T> || is_y<T> || is_angle<T> || is_scale<T>;
	}

namespace iige::ecs::components
	{
	struct parent
		{
		size_t children_count{0};
		entt::entity first_child{entt::null};
		};
	struct child
		{
		entt::entity parent{entt::null};
		entt::entity next_sibling{entt::null};
		entt::entity prev_sibling{entt::null};
		};
	}