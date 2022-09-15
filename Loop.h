#pragma once

#include <vector>
#include <functional>

#include <utils/memory.h>
#include <utils/logger.h>

#include "ecs/systems/move.h"
#include "ecs/systems/draw.h"
#include "ecs/systems/collision.h"
#include "input.h"
#include "scene.h"
#include "Systems_manager.h"

#include "window.h"

namespace iige
	{
	namespace loop
		{
		namespace details
			{
			class base_loop
				{
				protected:
					utils::observer_ptr<iige::scene            > scene          {nullptr};
					utils::observer_ptr<iige::window           > window         {nullptr};
					utils::observer_ptr<iige::systems_manager  > systems_manager{nullptr};
					utils::observer_ptr<ecs::systems::collision> collision      {nullptr};
					utils::observer_ptr<input::manager         > input          {nullptr};

					void step(float delta_time)
						{
						iige::scene& scene  {*this->scene };
						iige::window& window{*this->window};

						sf::Event event;
						while (window.poll_event(event)) { input->evaluate_event(event, scene, window, delta_time); }
						input->events_end(scene, window, delta_time);

						ecs::systems::move(scene, delta_time);
						(*collision)(scene);

						for (const auto& step_system : systems_manager->step)
							{
							step_system(scene, window, delta_time);
							}
						}
					void draw(float delta_time, float interpolation)
						{
						iige::scene & scene  {*this->scene };
						iige::window& window{*this->window};
						
						window.sf_window.clear();
						ecs::systems::interpolate(scene, interpolation);
						
						for (const auto& draw_system : systems_manager->draw)
							{
							draw_system(scene, window, delta_time, interpolation);
							}
						
						window.sf_window.display();
						}

				public:
					base_loop(iige::scene & scene, iige::window & window, iige::systems_manager& systems_manager, ecs::systems::collision & collision, input::manager& input) noexcept :
						scene{&scene}, window{&window}, systems_manager{&systems_manager}, collision{&collision}, input{&input}
						{}

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
				fixed_game_speed_variable_framerate(iige::scene& scene, iige::window& window, iige::systems_manager& systems_manager, ecs::systems::collision& collision, input::manager& input, float steps_per_second = 1.f, size_t max_frameskip = 5) noexcept :
					details::base_loop{scene, window, systems_manager, collision, input},
					steps_per_second{steps_per_second}, max_frameskip{max_frameskip}, step_delta_time{sf::seconds(1.f / steps_per_second)}
					{}

				virtual void run() final override
					{
					// https://dewitters.com/dewitters-gameloop/
					iige::scene & scene {*this->scene };
					iige::window& window{*this->window};

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
				fixed_fps_and_game_speed(iige::scene& scene, iige::window& window, iige::systems_manager& systems_manager, ecs::systems::collision& collision, input::manager& input, float steps_per_second = 1.f) noexcept :
					details::base_loop{scene, window, systems_manager, collision, input},
					steps_per_second{steps_per_second}, step_delta_time{sf::seconds(1.f / steps_per_second)}
					{}

				void run()
					{
					// https://dewitters.com/dewitters-gameloop/
					iige::scene & scene {*this->scene };
					iige::window& window{*this->window};

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
				variable_fps_and_game_speed(iige::scene& scene, iige::window& window, iige::systems_manager& systems_manager, ecs::systems::collision& collision, input::manager& input) noexcept :
					details::base_loop{scene, window, systems_manager, collision, input}
					{}

				void run()
					{
					// https://dewitters.com/dewitters-gameloop/
					iige::scene & scene {*this->scene };
					iige::window& window{*this->window};

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