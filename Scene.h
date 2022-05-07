#pragma once

#include "ecs/entt.h"
#include "ecs/components/spatial.h"

namespace iige
	{





	//template <typename ...Ts>
	class Scene
		{
		friend class entity;
		public:
			entt::entity create_entity() {  }
			entt::entity create_entity(iige::transform transform)
				{
				entt::entity identifier{ecs_registry.create()};
				ecs_registry.emplace<iige::ecs::components::transform::absolute::translation>(identifier, transform.translation);
				ecs_registry.emplace<iige::ecs::components::transform::absolute::rotation   >(identifier, transform.rotation);
				ecs_registry.emplace<iige::ecs::components::transform::absolute::scaling    >(identifier, transform.scaling);
				return identifier;
				}

			entt::registry ecs_registry;

			// TODO:
			// If a component is non-next absolute or relative, it MUST be const
			//

			template<typename component_t, typename... other_components_t, typename... excluded_ts>
			[[nodiscard]] entt::basic_view<entt::registry::entity_type, entt::get_t<component_t, other_components_t...>, entt::exclude_t<excluded_ts...>> view(entt::exclude_t<excluded_ts...> = {})
				{
				

				return ecs_registry.view<component_t, other_components_t..., excluded_ts...>();
				}

			template <typename component_t, typename ... arg_ts>
			component_t& emplace(entt::entity entity, arg_ts&& ... args)
				{
				return ecs_registry.emplace<component_t>(entity, std::forward<arg_ts>(args)...);
				}
			template <typename component_t, typename ... arg_ts>
			component_t& remove(entt::entity entity)
				{
				return ecs_registry.remove<component_t>(entity);
				}
		private:
			//utils::polymorphic_container<batch_container, Object, Ts...> inh_container;
		};
	}