#pragma once

#include <variant>
#include <cassert>

#include <utils/compilation/debug.h>
#include <utils/math/geometry/interactions.h>
#include <utils/math/geometry/transformations.h>
#include <utils/math/geometry/continuous_interactions.h>

// Left in memory of the pyramids
// #include <utils/concepts.h>
// #include <utils/math/vec2.h>
// #include <utils/math/vec2.h >
// #include <utils/math/vec2.h  >
// #include <utils/math/vec2.h   >
// #include <utils/math/vec2.h    >
// #include <utils/math/vec2.h     >
// #include <utils/math/transform2.h>
// #include <utils/math/transform2.h >
// #include <utils/math/transform2.h  >
// #include <utils/math/geometry/aabb.h>
// #include <utils/math/geometry/point.h>
// #include <utils/math/geometry/circle.h>
// #include <utils/math/geometry/polygon.h>
// #include <utils/math/geometry/polygon.h >
// #include <utils/math/geometry/polygon.h  >
// #include <utils/math/geometry/polygon.h   >
// #include <utils/math/geometry/polygon.h    >
// #include <utils/math/geometry/interactions.h>
// #include <utils/math/geometry/interactions.h >
// #include <utils/math/geometry/interactions.h  >
// #include <utils/math/geometry/transformations.h>
// #include <utils/math/geometry/continuous_point.h>
// #include <utils/math/geometry/continuous_point.h >
// #include <utils/math/geometry/continuous_point.h  >
// #include <utils/math/geometry/continuous_point.h   >
// #include <utils/math/geometry/continuous_point.h    >
// #include <utils/math/geometry/continuous_point.h     >
// #include <utils/math/geometry/continuous_point.h      >
// #include <utils/math/geometry/continuous_interactions.h>

#include <SFML/Graphics.hpp>

#include "../entt.h"

#include "../../types/shapes.h"
#include "../../types/core.h"

#include "component.h"
#include "../exceptions.h"
#include "spatial.h"
#include "../../Scene.h"

namespace iige::ecs::components
	{
	using namespace entt::literals;

#pragma region Collider types related stuff
	namespace colliders
		{
		template <typename T>
		concept is_discrete_collider = iige::shapes::shape_discrete<typename T::value_type>;

		namespace details
			{
			template <iige::shapes::shape_discrete T>
			using source = component<T, "collider_source"_hs, true>;
			
			template <iige::shapes::shape_discrete discrete_shape_t, bool is_hollow = false>
			struct discrete_collider : component<discrete_shape_t, "collider"_hs, true>
				{
				using component<discrete_shape_t, "collider"_hs, true>::component;
				using discrete_source = source<discrete_shape_t>;
				inline static constexpr bool hollow = is_hollow;
				};
			}

		using point                 = details::discrete_collider<iige::shapes::point         , false>;
		using segment               = details::discrete_collider<iige::shapes::segment       , false>;
		using aabb                  = details::discrete_collider<iige::shapes::aabb          , false>;
		using circle                = details::discrete_collider<iige::shapes::circle        , false>;
		using polygon               = details::discrete_collider<iige::shapes::polygon       , false>;
		using convex_polygon        = details::discrete_collider<iige::shapes::convex_polygon, false>;
		using hollow_aabb           = details::discrete_collider<iige::shapes::aabb          , true >;
		using hollow_circle         = details::discrete_collider<iige::shapes::circle        , true >;
		using hollow_polygon        = details::discrete_collider<iige::shapes::polygon       , true >;
		using hollow_convex_polygon = details::discrete_collider<iige::shapes::convex_polygon, true >;

		namespace details
			{
			template <iige::shapes::shape_continuous continuous_shape_t, typename discrete_shape_t, bool is_hollow = false>
			struct continuous_collider : component<continuous_shape_t, "collider"_hs, true>
				{
				using component<continuous_shape_t, "collider"_hs, true>::component;
				using discrete_source = source<discrete_shape_t>;
				inline static constexpr bool hollow = is_hollow;
				};
			}

		using continuous_point = details::continuous_collider<iige::shapes::continuous_point, iige::shapes::point>;
		
		template <typename T>
		concept is_continuous_collider = iige::shapes::shape_continuous<typename T::value_type>;

		template <typename T>
		concept is_collider = is_discrete_collider<T> || is_continuous_collider<T>;

		namespace details
			{
			//TODO ptr must have "hollow" boolean. If true discrete-discrete(hollow) collision checks only intersects, while continue-discrete(hollow) still check collides for the "inside" boolean to pass to the continuous check, but if hollow it won't add the discrete collided with component.
			using ptr   = std::variant
				<
				// Base
				utils::observer_ptr<point>, 
				utils::observer_ptr<segment>, 
				utils::observer_ptr<aabb>, 
				utils::observer_ptr<circle>, 
				utils::observer_ptr<polygon>, 
				utils::observer_ptr<convex_polygon>,
				// Hollow
				utils::observer_ptr<hollow_aabb>,
				utils::observer_ptr<hollow_circle>, 
				utils::observer_ptr<hollow_polygon>, 
				utils::observer_ptr<hollow_convex_polygon>,
				// Continuous
				utils::observer_ptr<continuous_point>
				// Continuous hollow
				>;
			}
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
		utils::math::geometry::collision_data data;
		};

#pragma endregion Collision flags

	/// <summary>
	/// Adds all the components required for collision checks. Requires the entity to already have a transform.
	/// is_static must be True if the collider does not move (regardless of the entity being able to move or not. False otherwise.
	/// </summary>
	template <colliders::is_collider collider_t, bool is_static = false, typename ...Args>
	inline void add_collision(entt::registry& registry, entt::entity entity, Args&&... args) noexcept
		{
		using namespace iige::shapes::transformations;

		auto& current_collider{registry.get_or_emplace<collider_t>(entity)};
		auto& current_collider_ptr{registry.emplace_or_replace<colliders::details::ptr>(entity, static_cast<collider_t*>(&current_collider))};

		if constexpr (!std::is_same_v<collider_t, colliders::aabb>)
			{
			utils::discard(registry.get_or_emplace<colliders::aabb>(entity, static_cast<shapes::aabb>(current_collider.value())));
			}

		if constexpr (!is_static)
			{
			utils::discard(registry.get_or_emplace<typename collider_t::discrete_source>(entity, std::forward<Args>(args)...));
			}
		}

	template <colliders::is_collider collider_t>
	inline void remove_collider(entt::registry& registry, entt::entity entity)
		{
		if (registry.all_of<         collider_t                 >(entity)) {
			registry.remove<         collider_t                 >(entity); }
		if (registry.all_of<typename collider_t::discrete_source>(entity)) { 
			registry.remove<typename collider_t::discrete_source>(entity); }
		}


	template <colliders::is_collider candidate, colliders::is_collider filter>
	inline void remove_if_not(entt::registry& registry, entt::entity entity)
		{
		if constexpr (!std::is_same_v<candidate, filter>)
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
				if (registry.all_of<collider_t::discrete_source>(entity)) { return; }
				}
			else { return; }
			}
		
		remove_if_not<colliders::point                , collider_t>(registry, entity);
		remove_if_not<colliders::segment              , collider_t>(registry, entity);
		remove_if_not<colliders::aabb                 , collider_t>(registry, entity);
		remove_if_not<colliders::circle               , collider_t>(registry, entity);
		remove_if_not<colliders::polygon              , collider_t>(registry, entity);
		remove_if_not<colliders::convex_polygon       , collider_t>(registry, entity);
		remove_if_not<colliders::hollow_aabb          , collider_t>(registry, entity);
		remove_if_not<colliders::hollow_circle        , collider_t>(registry, entity);
		remove_if_not<colliders::hollow_polygon       , collider_t>(registry, entity);
		remove_if_not<colliders::hollow_convex_polygon, collider_t>(registry, entity);
		remove_if_not<colliders::continuous_point     , collider_t>(registry, entity);
		
		add_collision<collider_t, is_static>(registry, entity, std::forward<Args>(args)...);
		}

	}
