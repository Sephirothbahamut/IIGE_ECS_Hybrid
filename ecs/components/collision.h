#pragma once

#include <variant>
#include <utils/math/vec2.h>
#include <utils/math/geometry/aabb.h>
#include <utils/math/geometry/circle.h>
#include <utils/math/geometry/polygon.h>
#include <utils/math/geometry/interactions.h>

#include <entt.h>

#include "spatial.h"

namespace iige::ecs::components
	{
	using transform = utils::math::Transform2;

	namespace 
		{
		namespace utmg = utils::math::geometry;
		namespace utm = utils::math;
		}

	using collider = std::variant<utm::vec2f, /*utmg::aabb,*/ utmg::circle, utmg::polygon>;
	collider  operator* (const collider& collider, const components::transform& transform) noexcept;
	collider& operator*=(      collider& collider, const components::transform& transform) noexcept;

	struct collider_source : collider //moving objects will keep the original collider stored here
		{
		using collider::collider;
		collider_source           (const collider& copy) noexcept : collider{copy} {}
		collider_source& operator=(const collider& copy) noexcept { collider::operator= (copy); return *this; }
		};

#pragma region Collision flags
	template <size_t layer>
	struct has_collision {};

	template <size_t layer>
	struct collides_with {};
#pragma endregion Collision flags

	struct collided_with { entt::entity other{entt::null}; };

	void add_collision(entt::registry& registry, entt::entity entity, const components::collider& collider = {});
	}
