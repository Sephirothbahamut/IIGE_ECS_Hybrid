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
	struct relative
		{
		struct translation : component<vec2f, "relative_translation"    > 
			{ 
			using component<vec2f, "relative_translation">::component;
			using min = component<vec2f, "relative_translation_min">; 
			using max = component<vec2f, "relative_translation_max">; 
			};
		struct rotation : component<iige::angle::rad, "relative_rotation"> 
			{
			using component<iige::angle::rad, "relative_rotation">::component;
			using min = component<iige::angle::rad, "relative_rotation_min">;
			using max = component<iige::angle::rad, "relative_rotation_max">;
			};
		struct scaling : component<float, "relative_scaling"> 
			{
			using component<float, "relative_scaling">::component;
			using min = component<float, "relative_scaling_min">; 
			using max = component<float, "relative_scaling_max">; 
			};
		struct next
			{
			struct translation : component<vec2f, "relative_next_translation"    >
				{
				using component<vec2f, "relative_next_translation">::component;
				using current_t = relative::translation;
				};
			struct rotation : component<iige::angle::rad, "relative_next_rotation"> 
				{ 
				using      component<iige::angle::rad, "relative_next_rotation">::component;
				using current_t = relative::rotation;
				};
			struct scaling : component<float           , "relative_next_scaling"> 
				{ 
				using      component<float           , "relative_next_scaling">::component;
				using current_t = relative::scaling;
				};
			};
		};
	struct absolute
		{
		struct translation : component<vec2f, "absolute_translation"    >
			{
			using component<vec2f, "absolute_translation">::component;
			using min = component<vec2f, "absolute_translation_min">;
			using max = component<vec2f, "absolute_translation_max">;
			};
		struct rotation : component<iige::angle::rad, "absolute_rotation">
			{
			using component<iige::angle::rad, "absolute_rotation">::component;
			using min = component<iige::angle::rad, "absolute_rotation_min">;
			using max = component<iige::angle::rad, "absolute_rotation_max">;
			};
		struct scaling : component<float, "absolute_scaling">
			{
			using component<float, "absolute_scaling">::component;
			using min = component<float, "absolute_scaling_min">;
			using max = component<float, "absolute_scaling_max">;
			};
		struct next
			{
			struct translation : component<vec2f, "absolute_next_translation"    >
				{
				using component<vec2f, "absolute_next_translation">::component;
				using current_t = absolute::translation;
				};
			struct rotation : component<iige::angle::rad, "absolute_next_rotation"> 
				{ 
				using      component<iige::angle::rad, "absolute_next_rotation">::component;
				using current_t = absolute::rotation;
				};
			struct scaling : component<float           , "absolute_next_scaling"> 
				{ 
				using      component<float           , "absolute_next_scaling">::component;
				using current_t = absolute::scaling;
				};
			};
		};
	struct interpolated
		{
		using translation = component<vec2f, "interpolated_translation">;
		using rotation = component<iige::angle::rad, "interpolated_rotation">;
		using scaling = component<float, "interpolated_scaling">;
		};
	struct speed
		{
		struct translation : component<vec2f, "speed_translation"    >
			{
			using component<vec2f, "speed_translation">::component;
			using min = component<vec2f, "speed_translation_min">;
			using max = component<vec2f, "speed_translation_max">;
			};
		struct directional : component<vec2f, "speed_directional"    >
			{
			using component<vec2f, "speed_directional">::component;
			using min = component<vec2f, "speed_directional_min">;
			using max = component<vec2f, "speed_directional_max">;
			};
		struct rotation : component<iige::angle::rad, "speed_rotation">
			{
			using component<iige::angle::rad, "speed_rotation">::component;
			using min = component<iige::angle::rad, "speed_rotation_min">;
			using max = component<iige::angle::rad, "speed_rotation_max">;
			};
		struct scaling : component<float, "speed_scaling">
			{
			using component<float, "speed_scaling">::component;
			using min = component<float, "speed_scaling_min">;
			using max = component<float, "speed_scaling_max">;
			};
		struct translation_magnitude
			{
			using min = component<float, "speed_translation_magnitude_min">;
			using max = component<float, "speed_translation_magnitude_max">;
			};
		};
	struct acceleration
		{
		struct translation : component<vec2f, "acceleration_translation"    >
			{
			using component<vec2f, "acceleration_translation">::component;
			using min = component<vec2f, "acceleration_translation_min">;
			using max = component<vec2f, "acceleration_translation_max">;
			};
		struct directional : component<vec2f, "acceleration_directional"    >
			{
			using component<vec2f, "acceleration_directional">::component;
			using min = component<vec2f, "acceleration_directional_min">;
			using max = component<vec2f, "acceleration_directional_max">;
			};
		struct rotation : component<iige::angle::rad, "acceleration_rotation">
			{
			using component<iige::angle::rad, "acceleration_rotation">::component;
			using min = component<iige::angle::rad, "acceleration_rotation_min">;
			using max = component<iige::angle::rad, "acceleration_rotation_max">;
			};
		struct scaling : component<float, "acceleration_scaling">
			{
			using component<float, "acceleration_scaling">::component;
			using min = component<float, "acceleration_scaling_min">;
			using max = component<float, "acceleration_scaling_max">;
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


	// Transform types
	template <typename T>
	concept is_absolute = ::utils::concepts::undecorated_any_of<T, absolute::translation, absolute::next::translation, absolute::rotation, absolute::next::rotation, absolute::scaling, absolute::next::scaling>;
	template <typename T>
	concept is_relative = ::utils::concepts::undecorated_any_of<T, relative::translation, relative::next::translation, relative::rotation, relative::next::rotation, relative::scaling, relative::next::scaling>;
	template <typename T>
	concept is_speed        = ::utils::concepts::undecorated_any_of<T, speed::translation, speed::directional, speed::rotation, speed::scaling>;
	template <typename T>
	concept is_acceleration = ::utils::concepts::undecorated_any_of < T, acceleration::translation, acceleration::directional, acceleration::rotation, acceleration::scaling > ;
	template <typename T>
	concept is_interpolated = ::utils::concepts::undecorated_any_of<T, interpolated::translation, interpolated::rotation, interpolated::scaling>;

	// Transform component types
	template <typename T>
	concept is_translation = ::utils::concepts::undecorated_any_of<T, absolute::translation, absolute::next::translation, relative::translation, relative::next::translation, speed::translation, acceleration::translation, interpolated::translation>;
	template <typename T>
	concept is_directional = ::utils::concepts::undecorated_any_of<T, speed::directional, acceleration::directional>;
	template <typename T>
	concept is_rotation = ::utils::concepts::undecorated_any_of<T, absolute::rotation, absolute::next::rotation, relative::rotation, relative::next::rotation, speed::rotation, acceleration::rotation, interpolated::rotation>;
	template <typename T>
	concept is_scaling = ::utils::concepts::undecorated_any_of<T, absolute::scaling, absolute::next::scaling, relative::scaling, relative::next::scaling, speed::scaling, acceleration::scaling, interpolated::scaling>;

	template <typename T>
	concept is_next = ::utils::concepts::undecorated_any_of
		<T, 
		absolute::next::translation, absolute::next::rotation, absolute::next::scaling, 
		relative::next::translation, relative::next::rotation, relative::next::scaling
		>;

	template <typename T>
	concept is_transform_component = is_translation<T> || is_rotation<T> || is_scaling<T>;
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