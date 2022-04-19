#pragma once

#include "ecs/entt.h"

namespace iige
	{

	//template <typename ...Ts>
	class Scene
		{
		public:
			entt::entity create_entity() { return ecs_registry.create(); }

			entt::registry ecs_registry;



			template<typename Component, typename... Other, typename... Exclude>
			[[nodiscard]] entt::basic_view<entt::registry::entity_type, entt::get_t<Component, Other...>, entt::exclude_t<Exclude...>> view(entt::exclude_t<Exclude...> = {})
				{
				return ecs_registry.view<Component, Other..., Exclude...>();
				}
		private:
			//utils::polymorphic_container<batch_container, Object, Ts...> inh_container;
		};
	}