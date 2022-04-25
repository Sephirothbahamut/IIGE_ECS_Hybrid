#pragma once

#include <utils/math/geometry/shapes.h>
#include <utils/math/geometry/transformations.h>
#include <utils/math/geometry/interactions.h>

namespace iige::shapes
	{
	using namespace utils::math::geometry;

	/*using aabb                 = utils::math::geometry::aabb;
	using circle               = utils::math::geometry::circle;
	using point                = utils::math::geometry::point;
	using polygon              = utils::math::geometry::polygon;
	using segment              = utils::math::geometry::segment;
	using convex_polygon       = utils::math::geometry::convex_polygon;
	using continuous_point     = utils::math::geometry::continuous_point;

	template <typename T> concept shape            = utils::math::geometry::shape           <T>;
	template <typename T> concept shape_continuous = utils::math::geometry::shape_continuous<T>;
	template <typename T> concept shape_discrete   = utils::math::geometry::shape_discrete  <T>;

	namespace transformations = utils::math::geometry::transformations;

	template <typename ... Args>
	auto collides(Args&& ... args) {
		return utils::math::geometry::collides(std::forward<Args>(args)...);
		}
	template <typename ... Args>
	auto continuous_collides(Args&& ... args) {
		return utils::math::geometry::continuous_collides(std::forward<Args>(args)...);
	}
	template <bool a_hollow = false, bool b_hollow = false, typename ... Args>
	auto contains(Args&& ... args) {
		return utils::math::geometry::contains<a_hollow, b_hollow>(std::forward<Args>(args)...);
		}
	template <typename ... Args>
	auto intersects(Args&& ... args) {
		return utils::math::geometry::intersects(std::forward<Args>(args)...);
		}*/
	}