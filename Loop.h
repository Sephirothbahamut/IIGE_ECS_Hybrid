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
	namespace loop
		{
		namespace details
			{
			class base_loop
				{
				protected:
					utils::observer_ptr<Scene> scene{nullptr};
					utils::observer_ptr<Window> window{nullptr};
					utils::observer_ptr<ecs::systems::collision> collision{nullptr};
					sf::VertexArray colliders_vertex_array{sf::PrimitiveType::Lines, 3600};

					void step(float delta_time)
						{
						Scene& scene  {*this->scene };
						Window& window{*this->window};

						sf::Event event;
						while (window.poll_event(event)) {}

						ecs::systems::move(scene, delta_time);
						(*collision)(scene);
						ecs::systems::update_colliders_vertex_array(scene, colliders_vertex_array);

						for (const auto& user_system : step_systems)
							{
							user_system(scene, window, delta_time);
							}
						}
					void draw(float delta_time, float interpolation)
						{
						Scene& scene  {*this->scene };
						Window& window{*this->window};

						window.sf_window.clear();
						ecs::systems::interpolate(scene, interpolation);
						ecs::systems::draw(scene, window.sf_window);
						window.sf_window.draw(colliders_vertex_array);

						for (const auto& draw_system : draw_systems)
							{
							draw_system(scene, window, delta_time, interpolation);
							}

						window.sf_window.display();
						}

				public:
					base_loop(Scene & scene, Window & window, ecs::systems::collision & collision) noexcept :
						scene{&scene}, window{&window}, collision{&collision}
						{}

					std::vector<std::function<void(Scene&, Window&, float       )>> step_systems;
					std::vector<std::function<void(Scene&, Window&, float, float)>> draw_systems;

					virtual void run() = 0;
				};
			}

		class fixed_game_speed_variable_framerate : public details::base_loop
			{
			private:
				const float steps_per_second = 1.f;
				const size_t max_frameskip = 5;

				sf::Time step_delta_time;

			public:
				fixed_game_speed_variable_framerate(Scene& scene, Window& window, ecs::systems::collision& collision, float steps_per_second = 1.f, size_t max_frameskip = 5) noexcept :
					details::base_loop{scene, window, collision},
					steps_per_second{steps_per_second}, max_frameskip{max_frameskip}, step_delta_time{sf::seconds(1.f / steps_per_second)}
					{}

				virtual void run() final override
					{
					// https://dewitters.com/dewitters-gameloop/
					Scene & scene {*this->scene };
					Window& window{*this->window};

					sf::Clock clock;
					sf::Time next_step_time{clock.getElapsedTime()};
					size_t step_loops{0};
					float interpolation{0};

					sf::Clock fps_clock;
					uint32_t frames_counter{0};

					while (window.is_open())
						{
						if (fps_clock.getElapsedTime() > sf::seconds(1))
							{
							utils::globals::logger.log("FPS: " + std::to_string(frames_counter / fps_clock.restart().asSeconds()) + " with #" + std::to_string(scene.ecs_registry.size()) + " active objects.");
							frames_counter = 0;
							}
						while (clock.getElapsedTime() > next_step_time && step_loops < max_frameskip)
							{
							step(step_delta_time.asSeconds());
							step_loops++;
							next_step_time += step_delta_time;
							}
						step_loops = 0;

						interpolation = (clock.getElapsedTime() + step_delta_time - next_step_time) / step_delta_time;

						frames_counter++;
						draw(step_delta_time.asSeconds(), interpolation);
						}
					}
			};
		
		class fixed_fps_and_game_speed : public details::base_loop
			{
			private:
				const float steps_per_second = 1.f;

				sf::Time step_delta_time;

			public:
				fixed_fps_and_game_speed(Scene& scene, Window& window, ecs::systems::collision& collision, float steps_per_second = 1.f) noexcept :
					details::base_loop{scene, window, collision},
					steps_per_second{steps_per_second}, step_delta_time{sf::seconds(1.f / steps_per_second)}
					{}

				void run()
					{
					// https://dewitters.com/dewitters-gameloop/
					Scene & scene {*this->scene };
					Window& window{*this->window};

					sf::Clock clock;

					sf::Time next_step_time = clock.getElapsedTime();

					sf::Time sleep_time{sf::seconds(0)};

					sf::Clock fps_clock;
					uint32_t frames_counter{0};

					while (window.is_open())
						{
						if (fps_clock.getElapsedTime() > sf::seconds(1))
							{
							utils::globals::logger.log("FPS: " + std::to_string(frames_counter / fps_clock.restart().asSeconds()) + " with #" + std::to_string(scene.ecs_registry.size()) + " active objects.");
							frames_counter = 0;
							}
						
						step(step_delta_time.asSeconds());

						frames_counter++;
						draw(step_delta_time.asSeconds(), 0);

						next_step_time += step_delta_time;
						sleep_time = next_step_time - clock.getElapsedTime();
						if (sleep_time >= sf::seconds(0)) { sf::sleep(sleep_time); }
						}
					}
			};
		class variable_fps_and_game_speed : public details::base_loop
			{
			private:

			public:
				variable_fps_and_game_speed(Scene& scene, Window& window, ecs::systems::collision& collision) noexcept :
					details::base_loop{scene, window, collision}
					{}

				void run()
					{
					// https://dewitters.com/dewitters-gameloop/
					Scene & scene {*this->scene };
					Window& window{*this->window};

					sf::Clock clock;

					sf::Time prev_step_time;
					sf::Time curr_step_time{clock.getElapsedTime()};

					sf::Time step_delta_time;

					sf::Clock fps_clock;
					uint32_t frames_counter{0};

					while (window.is_open())
						{
						if (fps_clock.getElapsedTime() > sf::seconds(1))
							{
							utils::globals::logger.log("FPS: " + std::to_string(frames_counter / fps_clock.restart().asSeconds()) + " with #" + std::to_string(scene.ecs_registry.size()) + " active objects.");
							frames_counter = 0;
							}
						
						prev_step_time = curr_step_time;
						curr_step_time = clock.getElapsedTime();

						step_delta_time = curr_step_time - prev_step_time;

						step(step_delta_time.asSeconds());
						frames_counter++;
						draw(step_delta_time.asSeconds(), 0);
						}
					}

			};
		
		enum class timing { fixed, variable };
		}
	}