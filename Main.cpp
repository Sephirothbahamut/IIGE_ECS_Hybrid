

#include <utils/cout_containers.h>
#include <utils/math/vec2.h>

#include <random>
#include <utils/definitions.h>
#include <utils/math/angle.h>

#include "Window.h"
#include "Scene.h"
#include "Loop.h"

#include "ecs/components/bad_draw.h"
#include "ecs/components/spatial.h"
#include "ecs/systems/collision.h"

#include <utils/math/geometry/continuous_interactions.h>
#include <iostream>

void main_shapes(iige::Window& window, iige::Scene& scene)
	{
	utils::math::transform2 t{.translation{200.f, 150.f}, .rotation{}};

	std::random_device rd;
	std::mt19937 mt{static_cast<long unsigned int>(0)};
	std::uniform_int_distribution <int>   distribution{0,    6};
	std::uniform_real_distribution<float> x_distribution{0, 1024};
	std::uniform_real_distribution<float> y_distribution{0,  768};
	std::uniform_real_distribution<float> angle_distribution{0,  360};
	std::uniform_real_distribution<float> rotation_distribution{-10,   10};
	std::uniform_real_distribution<float> speed_distribution{-100, 100};

	for (size_t i = 0; i < 800; i++)
		{
		auto entity{scene.ecs_registry.create()};

		iige::vec2f       position{x_distribution(mt), y_distribution(mt)};
		iige::angle::deg  angle{angle_distribution(mt)};
		iige::vec2f       speed{speed_distribution(mt), speed_distribution(mt)};
		iige::angle::deg  rotation{rotation_distribution(mt)};

		iige::ecs::components::transform::absolute::translation::min m;

		scene.ecs_registry.emplace<iige::ecs::components::transform::absolute::translation>(entity, position);
		scene.ecs_registry.emplace<iige::ecs::components::transform::absolute::next::translation>(entity, position);
		scene.ecs_registry.emplace<iige::ecs::components::transform::absolute::rotation>(entity, angle);
		scene.ecs_registry.emplace<iige::ecs::components::transform::absolute::next::rotation>(entity, angle);
		scene.ecs_registry.emplace<iige::ecs::components::transform::absolute::scaling>(entity, 1.f);
		scene.ecs_registry.emplace<iige::ecs::components::transform::absolute::next::scaling>(entity, 1.f);
		scene.ecs_registry.emplace<iige::ecs::components::transform::speed::translation>(entity, speed);
		scene.ecs_registry.emplace<iige::ecs::components::transform::speed::rotation>(entity, rotation);

		scene.ecs_registry.emplace<iige::ecs::components::transform::absolute::translation::min>(entity, 256.f, 0.f);
		scene.ecs_registry.emplace<iige::ecs::components::transform::speed::translation::max>(entity, 10.f, 0.f);
		scene.ecs_registry.emplace<iige::ecs::components::transform::acceleration::translation>(entity, -5.f, 0.f);

		scene.ecs_registry.emplace<iige::ecs::components::has_collision<0>>(entity);
		scene.ecs_registry.emplace<iige::ecs::components::collides_with<0>>(entity);

		int collider_type{distribution(mt)};
		switch (collider_type)
			{
			case 0:
				iige::ecs::components::add_collision<iige::ecs::components::colliders::polygon         >(scene.ecs_registry, entity, iige::shapes::polygon{{-10, -10}, {-5, 0}, { 10, -10 }, {10, 10}, {0, 0}, {-10, 10}});
				break;
			case 1:
				iige::ecs::components::add_collision<iige::ecs::components::colliders::convex_polygon  >(scene.ecs_registry, entity, std::vector<utils::math::vec2f>{ { 0, 0 }, {100, 50}, {50, 100}});
				break;
			case 2:
				iige::ecs::components::add_collision<iige::ecs::components::colliders::circle          >(scene.ecs_registry, entity, iige::shapes::circle{{0, 0}, 32});
				break;
			case 3:
				iige::ecs::components::add_collision<iige::ecs::components::colliders::aabb            >(scene.ecs_registry, entity, iige::shapes::aabb{.ll{-10}, .up{-10}, .rr{10}, .dw{10}});
				break;
			case 4:
				iige::ecs::components::add_collision<iige::ecs::components::colliders::segment         >(scene.ecs_registry, entity, iige::shapes::segment{{-10, 10}, {10, -10}});
				break;
			case 5:
				iige::ecs::components::add_collision<iige::ecs::components::colliders::point           >(scene.ecs_registry, entity, iige::shapes::point{0, 0});
				break;
			case 6:
				iige::ecs::components::add_collision<iige::ecs::components::colliders::continuous_point>(scene.ecs_registry, entity, iige::shapes::point{0, 0});
				{
				//auto& speed_x{scene.ecs_registry.get<iige::ecs::components::transform::speed::x>(entity)};
				//auto& speed_y{scene.ecs_registry.get<iige::ecs::components::transform::speed::y>(entity)};
				//speed_x.value() *= 10.f;
				//speed_y.value() *= 10.f;
				}
				break;
			}
		}
	}

void main_circle_escape(iige::Window& window, iige::Scene& scene)
	{
	utils::math::transform2 t{.translation{200.f, 150.f}, .rotation{}};

	std::random_device rd;
	std::mt19937 mt{static_cast<long unsigned int>(0)};
	std::uniform_int_distribution <int>   distribution{0,    6};
	std::uniform_real_distribution<float> x_distribution{0, 1024};
	std::uniform_real_distribution<float> y_distribution{0,  768};
	std::uniform_real_distribution<float> angle_distribution{0,  360};
	std::uniform_real_distribution<float> rotation_distribution{-10,   10};
	std::uniform_real_distribution<float> speed_distribution{-100, 100};
	for (size_t i = 0; i < 1; i++)
		{
		iige::shapes::circle outer;
		utils::math::vec2f pos{window.sf_window.getSize().x / 2.f, window.sf_window.getSize().y / 2.f};
		if (true)
			{// Outer square
			auto entity{scene.ecs_registry.create()};
		
			scene.ecs_registry.emplace<iige::ecs::components::transform::absolute::translation>(entity, pos);
		
			int collider_type{distribution(mt)};
			//iige::ecs::components::add_collision<iige::ecs::components::colliders::aabb>(scene.ecs_registry, entity, iige::shapes::aabb  {.ll{-256}, .up{-256}, .rr{256}, .dw{256}});
			
			outer = iige::shapes::circle{ pos, 256.f };
			iige::ecs::components::add_collision<iige::ecs::components::colliders::hollow_circle>(scene.ecs_registry, entity, iige::shapes::circle{{0, 0}, 256.f});
			
			scene.ecs_registry.emplace<iige::ecs::components::has_collision<0>>(entity);
			}
		if (true)
			{// Inner square
			auto entity{scene.ecs_registry.create()};

			scene.ecs_registry.emplace<iige::ecs::components::transform::absolute::translation>(entity, pos);

			int collider_type{distribution(mt)};
			//iige::ecs::components::add_collision<iige::ecs::components::colliders::polygon>(scene.ecs_registry, entity, iige::shapes::polygon{{-16, -16}, {16, -16}, {16, 16}, {-16, 16}});
			iige::ecs::components::add_collision<iige::ecs::components::colliders::circle>(scene.ecs_registry, entity, iige::shapes::circle{{0, 0}, 128.f});

			scene.ecs_registry.emplace<iige::ecs::components::has_collision<0>>(entity);
			}

		for (size_t i = 0; i < 1000; i++)
			{// Random bouncy projectile
			auto entity{ scene.ecs_registry.create() };

			utils::math::vec2f position{x_distribution(mt), y_distribution(mt)};
			utils::math::vec2f speed{speed_distribution(mt), speed_distribution(mt)};

			scene.ecs_registry.emplace<iige::ecs::components::transform::absolute      ::translation>(entity, position);
			scene.ecs_registry.emplace<iige::ecs::components::transform::speed         ::translation>(entity, speed   );
			scene.ecs_registry.emplace<iige::ecs::components::transform::absolute::next::translation>(entity, position);
			scene.ecs_registry.emplace<iige::ecs::components::transform::interpolated  ::translation>(entity, position);

			int collider_type{ distribution(mt) };

			iige::ecs::components::add_collision<iige::ecs::components::colliders::continuous_point>(scene.ecs_registry, entity, iige::shapes::point{ 0, 0 });

			auto& pippo = scene.ecs_registry.emplace<iige::ecs::components::bad_draw>(entity, 16.f);
			pippo.cs.setFillColor(iige::shapes::contains(outer, position) ? sf::Color::Blue : sf::Color::Green);

			scene.ecs_registry.emplace<iige::ecs::components::collides_with<0>>(entity);
			}
		}
	}

void main_hierarchy(iige::Window& window, iige::Scene& scene)
	{
	using namespace iige::angle::literals;

	auto entity_a{scene.ecs_registry.create()};
	auto entity_b{scene.ecs_registry.create()};
	//auto entity_c{scene.ecs_registry.create()};
	iige::shapes::convex_polygon shape_a{{0, -5}, {3, 4}, {-3, 4}};
	iige::shapes::convex_polygon shape_b{{-4, -4}, {4, -4}, {4, 4}, {-4, 4}};
	//iige::shapes::convex_polygon shape_c{{0, 5}, {-3, 4}, {3, 4}};

	iige::ecs::components::add_collision<iige::ecs::components::colliders::convex_polygon>(scene.ecs_registry, entity_a, shape_a);
	iige::ecs::components::add_collision<iige::ecs::components::colliders::convex_polygon>(scene.ecs_registry, entity_b, shape_b);

	scene.ecs_registry.emplace<iige::ecs::components::transform::interpolated::rotation      >(entity_a);
	scene.ecs_registry.emplace<iige::ecs::components::transform::interpolated::translation>(entity_b);
	scene.ecs_registry.emplace<iige::ecs::components::transform::interpolated::rotation      >(entity_b);
	//scene.ecs_registry.emplace<iige::ecs::components::transform::interpolated::x>(entity_c);
	//scene.ecs_registry.emplace<iige::ecs::components::transform::interpolated::y>(entity_c);

	scene.ecs_registry.emplace<iige::ecs::components::transform::absolute::translation>(entity_a, window.sf_window.getSize().x / 2.f, window.sf_window.getSize().y / 2.f);
	scene.ecs_registry.emplace<iige::ecs::components::transform::absolute      ::rotation>(entity_a);
	scene.ecs_registry.emplace<iige::ecs::components::transform::absolute::next::rotation>(entity_a);
	scene.ecs_registry.emplace<iige::ecs::components::transform::speed         ::rotation>(entity_a, 45_deg);

	scene.ecs_registry.emplace<iige::ecs::components::transform::absolute::translation>(entity_b);
	scene.ecs_registry.emplace<iige::ecs::components::transform::absolute::rotation>(entity_b);
	scene.ecs_registry.emplace<iige::ecs::components::transform::absolute::next::translation>(entity_b);
	scene.ecs_registry.emplace<iige::ecs::components::transform::absolute::next::rotation>(entity_b);
	//scene.ecs_registry.emplace<iige::ecs::components::transform::absolute::x>(entity_c);
	//scene.ecs_registry.emplace<iige::ecs::components::transform::absolute::y>(entity_c);

	scene.ecs_registry.emplace<iige::ecs::components::transform::relative::translation>(entity_b);
	scene.ecs_registry.emplace<iige::ecs::components::transform::relative::rotation>(entity_b);
	scene.ecs_registry.emplace<iige::ecs::components::transform::relative::next::translation>(entity_b);
	scene.ecs_registry.emplace<iige::ecs::components::transform::relative::next::rotation>(entity_b);

	//scene.ecs_registry.emplace<iige::ecs::components::transform::relative      ::x>(entity_c, window.sf_window.getSize().x / 2.f);
	//scene.ecs_registry.emplace<iige::ecs::components::transform::relative      ::y>(entity_c, window.sf_window.getSize().y / 2.f);

	//scene.ecs_registry.emplace<iige::ecs::components::transform::relative::next::x>(entity_c, window.sf_window.getSize().x / 2.f);
	//scene.ecs_registry.emplace<iige::ecs::components::transform::relative::next::y>(entity_c, window.sf_window.getSize().y / 2.f);

	scene.ecs_registry.emplace<iige::ecs::components::transform::speed::translation>(entity_b, 5.f, 0.f);
	//scene.ecs_registry.emplace<iige::ecs::components::transform::speed         ::x>(entity_c, window.sf_window.getSize().x / 2.f);
	//scene.ecs_registry.emplace<iige::ecs::components::transform::speed         ::y>(entity_c, window.sf_window.getSize().y / 2.f);


	scene.ecs_registry.emplace<iige::ecs::components::child>(entity_b, entity_a);

	scene.ecs_registry.emplace<iige::ecs::components::has_collision<0>>(entity_a);
	auto& pippo = scene.ecs_registry.emplace<iige::ecs::components::bad_draw>(entity_a, 16.f);
	pippo.cs.setFillColor(sf::Color::Red);
	auto& pluto = scene.ecs_registry.emplace<iige::ecs::components::bad_draw>(entity_b, 16.f);
	pluto.cs.setFillColor(sf::Color::Blue);
	}

int main()
	{
	using namespace utils::math::angle::literals;
	
	iige::Window window{iige::Window::create_info{.size{1024, 768}, .position{10, 10}, .title{"My window"}}};
	
	iige::Scene scene;
	
	main_shapes(window, scene);

	iige::ecs::systems::collision_impl<1> collision;

	//iige::loop::variable_fps_and_game_speed loop{scene, window, collision};/*/
	iige::loop::fixed_game_speed_variable_framerate loop{scene, window, collision, 10};/**/


	//Entities bouncing system
	loop.step_systems.emplace_back([&](iige::Scene& scene, iige::Window& window, float delta_time)
		{
		auto view{scene.ecs_registry.view<iige::ecs::components::collision_data, iige::ecs::components::transform::speed::translation, iige::ecs::components::transform::absolute::next::translation>()};

		view.each([&](const iige::ecs::components::collision_data& cdata, iige::vec2f& speed_translation, iige::vec2f& next_translation)
			{
			using namespace utils::math::operators;
		
			speed_translation = speed_translation - cdata.data.normal * 2.f * (speed_translation <dot> cdata.data.normal);
		
			next_translation = cdata.data.impact_point + speed_translation * (1 - cdata.data.t) * delta_time;
			});
		});

	//Window wall bouncing system
	loop.step_systems.emplace_back([&](iige::Scene& scene, iige::Window& window, float)
		{
		auto view{scene.ecs_registry.view<iige::ecs::components::transform::absolute::translation, iige::ecs::components::transform::speed::translation>()};

		view.each([&](const iige::vec2f& absolute_translation, iige::vec2f& speed_translation)
			{
			auto&       x{absolute_translation.x};
			auto&       y{absolute_translation.y};
			auto& speed_x{speed_translation   .x};
			auto& speed_y{speed_translation   .y};
			if (x < 0 && speed_x < 0) { speed_x *= -1; }
			if (x > window.sf_window.getSize().x && speed_x > 0) { speed_x *= -1; }
			if (y < 0 && speed_y < 0) { speed_y *= -1; }
			if (y > window.sf_window.getSize().y && speed_y > 0) { speed_y *= -1; }
			});
		});
	
	loop.run();
	
	return 0;
	}
