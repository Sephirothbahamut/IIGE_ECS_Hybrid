#pragma once

#include <functional>

#include <utils/containers/handled_container.h>

#include "ecs/systems/system.h"

namespace iige
	{
	class scene;
	class window;
	namespace loop::details
		{
		class base_loop;
		}

	class systems_manager
		{
		friend class loop::details::base_loop;
		using callable_step_t = std::function<void(iige::scene&, iige::window&, float)>;
		using callable_draw_t = std::function<void(iige::scene&, iige::window&, float, float)>;
		using container_step_t = utils::containers::handled_container<callable_step_t>;
		using container_draw_t = utils::containers::handled_container<callable_draw_t>;

		public:
			using handle_step = container_step_t::handle_t;
			using handle_draw = container_draw_t::handle_t;

			template <typename T>
				requires std::derived_from<T, iige::ecs::systems::system_step> || std::derived_from<T, iige::ecs::systems::system_draw>
			auto emplace(T & system_class_instance)
				{
				if constexpr (std::derived_from<T, iige::ecs::systems::system_step> && std::derived_from<T, iige::ecs::systems::system_draw>)
					{
					return std::pair<handle_step, handle_draw>
						{
						step.emplace([&system_class_instance](iige::scene& scene, iige::window& window, float delta_time)
							{ system_class_instance.step(scene, window, delta_time); }),
						draw.emplace([&system_class_instance](iige::scene& scene, iige::window& window, float delta_time, float interpolation)
							{
							system_class_instance.draw(scene, window, delta_time, interpolation);
							}),
						};
					}
				else if constexpr (std::derived_from<T, iige::ecs::systems::system_step>) 
					{
					return step.emplace([&system_class_instance](iige::scene& scene, iige::window& window, float delta_time)
						{
						system_class_instance.step(scene, window, delta_time);
						});
					}
				else if constexpr (std::derived_from<T, iige::ecs::systems::system_draw>) 
					{
					return draw.emplace([&system_class_instance](iige::scene& scene, iige::window& window, float delta_time, float interpolation)
						{
						system_class_instance.draw(scene, window, delta_time, interpolation);
						});
					}
				}

			void remove(std::pair<handle_step, handle_draw>& handles) { step.remove(handles.first); draw.remove(handles.second); }

			container_step_t step;
			container_draw_t draw;

		private:

			handle_step emplace_step(std::derived_from<iige::ecs::systems::system_step> auto& system_class_instance)
				{
				return emplace_step([&](iige::scene& scene, iige::window& window, float delta_time) { system_class_instance.step(scene, window, delta_time); });
				}
			handle_draw emplace_draw(std::derived_from<iige::ecs::systems::system_draw> auto& system_class_instance)
				{
				return emplace_draw([&](iige::scene& scene, iige::window& window, float delta_time, float interpolation) { system_class_instance.draw(scene, window, delta_time, interpolation); });
				}
		};
	}