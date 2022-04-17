#pragma once

#include <utils/math/geometry/shapes.h>
#include <utils/math/geometry/transformations.h>
#include <utils/math/geometry/interactions.h>

namespace iige::shapes
	{
	using aabb             = utils::math::geometry::aabb;
	using circle           = utils::math::geometry::circle;
	using point            = utils::math::geometry::point;
	using polygon          = utils::math::geometry::polygon;
	using segment          = utils::math::geometry::segment;
	using convex_polygon   = utils::math::geometry::convex_polygon;
	using continuous_point = utils::math::geometry::continuous_point;

	namespace transformations = utils::math::geometry::transformations;

	template <typename ... Args>
	auto collides(Args&& ... args) {
		return utils::math::geometry::collides(std::forward<Args>(args)...);
		}
	template <typename ... Args>
	auto contains(Args&& ... args) {
		return utils::math::geometry::contains(std::forward<Args>(args)...);
		}
	template <typename ... Args>
	auto intersects(Args&& ... args) {
		return utils::math::geometry::intersects(std::forward<Args>(args)...);
		}
	}