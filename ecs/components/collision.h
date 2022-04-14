#pragma once

#include <variant>
#include <cassert>

#include <utils/compilation/debug.h>
#include <utils/concepts.h>
#include <utils/math/vec2.h>
#include <utils/math/transform2.h>
#include <utils/math/geometry/aabb.h>
#include <utils/math/geometry/circle.h>
#include <utils/math/geometry/polygon.h>
#include <utils/math/geometry/interactions.h>
#include <utils/math/geometry/transformations.h>

#include "../entt.h"

#include <SFML/Graphics.hpp>

#include "../exceptions.h"
#include "spatial.h"
#include "../../Scene.h"

namespace iige::ecs::components
	{
	using transform = ::utils::math::transform2;

	namespace
		{
		namespace utmg = ::utils::math::geometry;
		namespace utm = ::utils::math;
		}

#pragma region Collider types related stuff
	namespace colliders
		{
		struct segment        { segment       (utmg::segment       && data) : data{std::move(data)} {} utmg::segment        data; static constexpr auto in_place_delete = true; };
		struct aabb           { aabb          (utmg::aabb          && data) : data{std::move(data)} {} utmg::aabb           data; static constexpr auto in_place_delete = true; };
		struct circle         { circle        (utmg::circle        && data) : data{std::move(data)} {} utmg::circle         data; static constexpr auto in_place_delete = true; };
		struct polygon        { polygon       (utmg::polygon       && data) : data{std::move(data)} {} utmg::polygon        data; static constexpr auto in_place_delete = true; };
		//struct convex_polygon { convex_polygon(utmg::convex_polygon&& data) : data{std::move(data)} {} utmg::convex_polygon data; static constexpr auto in_place_delete = true; };

		template <typename T>
		concept is_collider = ::utils::concepts::any_of<T, colliders::segment, colliders::aabb, colliders::circle, colliders::polygon/*, colliders::convex_polygon*/>;

		using ptr = std::variant<segment*, aabb*, circle*, polygon*/*, convex_polygon**/>;

		template <is_collider collider_t>
		struct source : collider_t//moving objects will keep the original collider stored here
			{
			source(const collider_t& copy) noexcept : collider_t{copy} {}
			source& operator=(const collider_t& copy) noexcept { collider_t::operator=(copy); return *this; }
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

	struct collided
		{
		/// <summary> The entity hit. </summary>
		entt::entity other{entt::null};
		/// <summary> The point along other's edge that was impacted. </summary>
		utm::vec2f impact_point;
		/// <summary> The transform at which this entity impacts other. </summary>
		utm::transform2 impact_transform;
		/// <summary> The normal at the impact point. </summary>
		utm::vec2f normal;
		float time{0};
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

		auto& current_collider    {registry.get_or_emplace<collider_t>(entity, std::forward<Args>(args)...)};
		auto& current_collider_ptr{registry.get_or_emplace<colliders::ptr>(entity, &current_collider)};

		if constexpr (!std::is_same_v<collider_t, colliders::aabb>)
			{
			utils::discard(registry.get_or_emplace<colliders::aabb>(entity, static_cast<utmg::aabb>(current_collider.data)));
			}

		if constexpr(!is_static)
			{
			utils::discard(registry.get_or_emplace<colliders::source<collider_t>>(entity, current_collider));
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

		if constexpr (!std::is_same<collider_t, colliders::segment>) { if(registry.all_of<colliders::segment, colliders::source<colliders::segment>>(entity)) { registry.remove<colliders::segment>(entity); } }
		if constexpr (!std::is_same<collider_t, colliders::circle >) { if(registry.all_of<colliders::circle , colliders::source<colliders::circle >>(entity)) { registry.remove<colliders::circle >(entity); } }
		if constexpr (!std::is_same<collider_t, colliders::polygon>) { if(registry.all_of<colliders::polygon, colliders::source<colliders::polygon>>(entity)) { registry.remove<colliders::polygon>(entity); } }

		add_collision<collider_t, is_static, Args...>(registry, entity, std::forward(args)...);
		}

	}
