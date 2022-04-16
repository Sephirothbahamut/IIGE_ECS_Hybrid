#pragma once

#include <variant>
#include <cassert>

#include <utils/compilation/debug.h>

#include <utils/concepts.h>
#include <utils/math/vec2.h>
#include <utils/math/vec2.h >
#include <utils/math/vec2.h  >
#include <utils/math/vec2.h   >
#include <utils/math/vec2.h    >
#include <utils/math/vec2.h     >
#include <utils/math/transform2.h>
#include <utils/math/transform2.h >
#include <utils/math/transform2.h  >
#include <utils/math/geometry/aabb.h>
#include <utils/math/geometry/point.h>
#include <utils/math/geometry/circle.h>
#include <utils/math/geometry/polygon.h>
#include <utils/math/geometry/polygon.h >
#include <utils/math/geometry/polygon.h  >
#include <utils/math/geometry/polygon.h   >
#include <utils/math/geometry/polygon.h    >
#include <utils/math/geometry/interactions.h>
#include <utils/math/geometry/interactions.h >
#include <utils/math/geometry/interactions.h  >
#include <utils/math/geometry/transformations.h>
#include <utils/math/geometry/continuous_point.h>
#include <utils/math/geometry/continuous_point.h >
#include <utils/math/geometry/continuous_point.h  >
#include <utils/math/geometry/continuous_point.h   >
#include <utils/math/geometry/continuous_point.h    >
#include <utils/math/geometry/continuous_point.h     >
#include <utils/math/geometry/continuous_point.h      >
#include <utils/math/geometry/continuous_interactions.h>

#include <SFML/Graphics.hpp>

#include "../entt.h"

#include "component.h"
#include "../exceptions.h"
#include "spatial.h"
#include "../../Scene.h"

namespace iige::ecs::components
	{
	using transform = ::utils::math::transform2;

	namespace
		{
		namespace utmg = ::utils::math::geometry;
		namespace utm  = ::utils::math;
		}

#pragma region Collider types related stuff
	namespace colliders
		{
		template <typename shape_t>
		using discrete_collider = component_in_place_delete<shape_t>;


		using  point          = discrete_collider<utmg::point         >;
		using  segment        = discrete_collider<utmg::segment       >;
		using  aabb           = discrete_collider<utmg::aabb          >;
		using  circle         = discrete_collider<utmg::circle        >;
		using  polygon        = discrete_collider<utmg::polygon       >;
		using  convex_polygon = discrete_collider<utmg::convex_polygon>;

		template <typename T>
		concept is_discrete_collider = ::utils::concepts::any_of<T, point, segment, aabb, circle, polygon, convex_polygon>;

		template <typename continuous_shape_t, is_discrete_collider discrete_shape_t>
		struct continuous_collider : component_in_place_delete<continuous_shape_t>
			{
			using component_in_place_delete<continuous_shape_t>::component_in_place_delete;
			using discrete = discrete_shape_t;
			};

		using continuous_point = continuous_collider<utmg::continuous_point, point>;
		

		template <typename T>
		concept is_continuous_collider = ::utils::concepts::any_of<T, continuous_point>;

		template <typename T>
		concept is_collider = is_discrete_collider<T> || is_continuous_collider<T>;

		using ptr   = std::variant<point*, segment*, aabb*, circle*, polygon*, convex_polygon*, continuous_point*>;

		template <is_discrete_collider collider_t>
		struct source : collider_t//moving objects will keep the original collider stored here
			{
			using collider_t::collider_t;
			//source(const collider_t& copy) noexcept : collider_t{copy} {}
			//source& operator=(const collider_t& copy) noexcept { collider_t::operator=(copy); return *this; }
			};
		}

#pragma endregion Collider types related stuff


#pragma region Collision flags
	template <size_t layer>
	struct has_collision {};

	template <size_t layer>
	struct collides_with {};
#pragma endregion Collision flags

#pragma region Collision results
	struct collided_with { entt::entity other{entt::null}; };

	struct collision_data
		{
		entt::entity other{entt::null};
		utmg::collision_data data;
		};

#pragma endregion Collision flags

	/// <summary>
	/// Adds all the components required for collision checks. Requires the entity to already have a transform.
	/// is_static must be True if the collider does not move (regardless of the entity being able to move or not. False otherwise.
	/// </summary>
	template <colliders::is_collider collider_t, bool is_static = false, typename ...Args>
	inline void add_collision(entt::registry& registry, entt::entity entity, Args&&... args) noexcept
		{
		using namespace utils::math::geometry::transformations;

		auto& current_collider{registry.get_or_emplace<collider_t>(entity)};
		auto& current_collider_ptr{registry.get_or_emplace<colliders::ptr>(entity, static_cast<collider_t*>(&current_collider))};

		if constexpr (!std::is_same_v<collider_t, colliders::aabb>)
			{
			utils::discard(registry.get_or_emplace<colliders::aabb>(entity, static_cast<utmg::aabb>(current_collider.value())));
			}

		if constexpr (!is_static)
			{
			if constexpr (colliders::is_discrete_collider<collider_t>)
				{
				utils::discard(registry.get_or_emplace<colliders::source<collider_t>>(entity, std::forward<Args>(args)...));
				}
			else if constexpr (colliders::is_continuous_collider<collider_t>)
				{
				utils::discard(registry.get_or_emplace<colliders::source<collider_t::discrete>>(entity, std::forward<Args>(args)...));
				}
			}
		}

	template <colliders::is_collider collider_t>
	inline void remove_collider(entt::registry& registry, entt::entity entity)
		{
		if constexpr (colliders::is_discrete_collider<collider_t>)
			{
			if (registry.all_of<                  collider_t >) { registry.remove<                  collider_t >(entity); }
			if (registry.all_of<colliders::source<collider_t>>) { registry.remove<colliders::source<collider_t>>(entity); }
			}
		else if constexpr (colliders::is_continuous_collider<collider_t>)
			{
			if (registry.all_of<                  collider_t           >) { registry.remove<                  collider_t           >(entity); }
			if (registry.all_of<colliders::source<collider_t::discrete>>) { registry.remove<colliders::source<collider_t::discrete>>(entity); }
			}
		}


	template <colliders::is_collider candidate, colliders::is_collider filter>
	inline void remove_if_not(entt::registry& registry, entt::entity entity)
		{
		if constexpr (!std::is_same<candidate, filter>)
			{
			remove_collider<candidate>(registry, entity);
			}
		}

	template <colliders::is_collider collider_t, bool is_static = false, typename ...Args>
	inline void add_or_replace_collision(entt::registry& registry, entt::entity entity, Args&&... args) noexcept
		{
		if (registry.all_of<collider_t, colliders::aabb>(entity)) 
			{
			if constexpr (!is_static) 
				{
				if (registry.all_of<colliders::source<collider_t>>(entity)) { return; }
				}
			else { return; }
			}

		remove_if_not<colliders::point           , collider_t>(registry, entity);
		remove_if_not<colliders::segment         , collider_t>(registry, entity);
		remove_if_not<colliders::circle          , collider_t>(registry, entity);
		remove_if_not<colliders::polygon         , collider_t>(registry, entity);
		remove_if_not<colliders::convex_polygon  , collider_t>(registry, entity);
		remove_if_not<colliders::continuous_point, collider_t>(registry, entity);
		
		add_collision<collider_t, is_static, Args...>(registry, entity, std::forward(args)...);
		}

	}
