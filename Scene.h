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
		private:
			//utils::polymorphic_container<batch_container, Object, Ts...> inh_container;
		};
	}