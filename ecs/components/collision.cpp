#include "collision.h"
#include "spatial.h"

namespace iige::ecs::components
	{
	collider operator*(const collider& collider, const components::transform& transform) noexcept
		{
		using namespace utils::math::geometry::transformations;
		return std::visit([&](const auto& collider)->components::collider { return collider * transform; }, collider);
		}
	collider& operator*=(collider& collider, const components::transform& transform) noexcept
		{
		using namespace utils::math::geometry::transformations;
		return collider = std::visit([&](const auto& collider)->components::collider { return collider * transform; }, collider);
		}

	void add_collision(entt::registry& registry, entt::entity entity, const components::collider& collider)
		{
		auto& current_collider{registry.get_or_emplace<components::collider>(entity, collider)};

		components::transform* transform{registry.try_get<components::transform>(entity)};
		if (transform)
			{
			utils::discard(registry.get_or_emplace<components::collider_source>(entity, collider));
			current_collider *= *transform;
			}
		}
	}
