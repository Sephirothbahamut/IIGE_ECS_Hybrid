#pragma once

#include <vector>
#include <functional>

#include <utils/memory.h>
#include <utils/logger.h>

#include "ecs/systems/move.h"
#include "ecs/systems/draw.h"
#include "ecs/systems/collision.h"
#include "Scene.h"

#include "Window.h"

namespace iige
	{
	class Loop
		{
		private:
			static constexpr bool log_enabled = true;

			utils::observer_ptr<Scene> scene {nullptr};
			utils::observer_ptr<Window> window{nullptr};
			utils::observer_ptr<ecs::systems::collision> collision{nullptr};

			const float steps_per_second = 1.f;
			const sf::Time fixed_delta_time{sf::seconds(1.f / steps_per_second)};
			const size_t max_frameskip = 5;

		public:
			Loop(Scene& scene, Window& window, ecs::systems::collision& collision, float steps_per_second = 1.f) noexcept : scene{&scene}, window{&window}, collision{&collision}, steps_per_second{steps_per_second}
				{}

			std::vector<std::function<void(Scene&, Window&)>> user_systems;

			void run()
				{
				// https://dewitters.com/dewitters-gameloop/
				Scene& scene  = *this->scene;
				Window&  window = *this->window;

				sf::VertexArray colliders_vertex_array{sf::PrimitiveType::Lines};

				sf::Clock clock;
				sf::Time next_step_time = clock.getElapsedTime();
				size_t step_loops = 0;
				float interpolation = 0;

				sf::Clock fps_clock;
				uint32_t frames_counter = 0;

				while (window.is_open())
					{
					if (fps_clock.getElapsedTime() > sf::seconds(1))
						{
						utils::globals::logger.log("FPS: " + std::to_string(frames_counter / fps_clock.restart().asSeconds()) + " with #" + std::to_string(scene.ecs_registry.size()) + " active objects.");
						frames_counter = 0;
						}
					while (clock.getElapsedTime() > next_step_time && step_loops < max_frameskip)
						{
						sf::Event event;
						while (window.poll_event(event)) {}

						ecs::systems::move(scene);
						ecs::systems::update_colliders_vertex_array(scene, colliders_vertex_array);
						(*collision)(scene);

						// User/gameplay systems
						for (const auto& user_system : user_systems)
							{
							user_system(scene, window);
							}


						next_step_time += fixed_delta_time;
						}

					interpolation = (clock.getElapsedTime() + fixed_delta_time - next_step_time) / fixed_delta_time;

					frames_counter++;
					window.sf_window.clear();
					ecs::systems::interpolate(scene, interpolation);
					ecs::systems::draw(scene, window.sf_window);
					window.sf_window.draw(colliders_vertex_array);
					window.sf_window.display();
					}
				}
		};
	}