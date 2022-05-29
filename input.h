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
	}

namespace iige::input
	{


	class button
		{
		public:
			enum class state { up, down };
			using on_state_callable_t = std::function<void(iige::scene&, iige::window&, float)>;

			using container_on_state_t        = utils::containers::handled_container<on_state_callable_t>;

			container_on_state_t pressed;
			container_on_state_t released;
			container_on_state_t up;
			container_on_state_t down;

		private:

			void set_state(state state, iige::scene& scene, iige::window& window, float delta_time)
				{
				if (state != current_state)
					{
					if (state == iige::input::button::state::up)
						{
						for (auto& callback : pressed) 
							{
							callback(scene, window, delta_time); 
							}
						}
					else
						{
						for (auto& callback : released)
							{
							callback(scene, window, delta_time);
							}
						}

					current_state = state;
					}
				}
			state current_state;
		};



	}