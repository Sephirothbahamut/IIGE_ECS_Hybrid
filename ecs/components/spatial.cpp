#include "collision.h"
#include "spatial.h"

namespace iige::ecs::components
	{
	void add_movement(entt::registry& registry, entt::entity entity, components::transform transform, components::transform speed)
		{
		const auto& current_transform{registry.get_or_emplace<components::transform>(entity, transform)};
		utils::discard(registry.get_or_emplace<transform_next>(entity, current_transform));
		utils::discard(registry.get_or_emplace<transform_prev>(entity, current_transform));
		utils::discard(registry.get_or_emplace<components::speed>(entity, speed));

		/*components::collider* collider{registry.try_get<components::collider>(entity)};
		if (collider)
			{
			utils::discard(registry.get_or_emplace<components::collider_source>(entity, *collider));

			(*collider) &= transform;
			}TODO*/
		}
	}
