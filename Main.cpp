


#include <random>
#include <utils/definitions.h>
#include <utils/math/angle.h>

#include "Window.h"
#include "Scene.h"
#include "Loop.h"

#include "ecs/components/bad_draw.h"
#include "ecs/components/spatial.h"
#include "ecs/systems/collision.h"

int main()
	{
	using namespace utils::angle::literals;
	
	iige::Window window{iige::Window::create_info{.size{1024, 768}, .position{10, 10}, .title{"My window"}}};
	
	iige::Scene scene;
	
	utils::math::Transform2 t{.position{200.f, 150.f}, .orientation{}};

	std::random_device rd;
	std::mt19937 mt{static_cast<long unsigned int>(0)};
	std::uniform_int_distribution<int> distribution{0, 4};
	std::uniform_real_distribution<float> x_distribution{0, 1024};
	std::uniform_real_distribution<float> y_distribution{0,  768};
	std::uniform_real_distribution<float> angle_distribution{0, 360};
	std::uniform_real_distribution<float> rotation_distribution{-10, 10};
	std::uniform_real_distribution<float> speed_distribution{-10, 10};
	
	for (size_t i = 0; i < 800; i++)
		{
		auto entity{scene.ecs_registry.create()};

		utils::math::vec2f position{x_distribution(mt), y_distribution(mt)};
		utils::angle::deg  angle   {angle_distribution(mt)};
		utils::math::vec2f speed   {speed_distribution (mt), speed_distribution (mt)};
		utils::angle::deg  rotation{rotation_distribution(mt)};

		iige::ecs::components::add_movement(scene.ecs_registry, entity, {position, angle}, {speed, rotation});

		int collider_type{distribution(mt)};
		switch (collider_type)
			{
			case 0:
				iige::ecs::components::add_collision<iige::ecs::components::colliders::polygon>(scene.ecs_registry, entity, iige::ecs::components::utmg::polygon{{-10, -10}, {-5, 0}, { 10, -10 }, {10, 10}, {0, 0}, {-10, 10}});
				break;
			case 1:
				iige::ecs::components::add_collision<iige::ecs::components::colliders::polygon>(scene.ecs_registry, entity, iige::ecs::components::utmg::convex_polygon{{0, 0}, {100, 50}, {50, 100}});
				break;
			case 2:
				iige::ecs::components::add_collision<iige::ecs::components::colliders::circle >(scene.ecs_registry, entity, iige::ecs::components::utmg::circle{{0, 0}, 32});
				break;
			case 3:
				iige::ecs::components::add_collision<iige::ecs::components::colliders::aabb   >(scene.ecs_registry, entity, iige::ecs::components::utmg::aabb{.ll{-10}, .up{-10}, .rr{10}, .dw{10}});
				break;
			case 4:
				iige::ecs::components::add_collision<iige::ecs::components::colliders::segment >(scene.ecs_registry, entity, iige::ecs::components::utmg::segment{{-10, 10}, {10, -10}});
				break;
			case 5:
				//iige::ecs::components::add_collision(scene.ecs_registry, entity, iige::ecs::components::collider{utils::math::vec2f{0, 0}});
				break;
			}

		
		scene.ecs_registry.emplace<iige::ecs::components::has_collision<0>>(entity);
		scene.ecs_registry.emplace<iige::ecs::components::collides_with<0>>(entity);
	
		//scene.ecs_registry.emplace<iige::ecs::components::bad_draw>(entity, 32.f);
		}
	
	iige::ecs::systems::collision_impl<2> collision;
	
	iige::loop::fixed_game_speed_variable_framerate loop{scene, window, collision, 60.f};


	//Bouncing system
	loop.step_systems.emplace_back([&](iige::Scene& scene, iige::Window& window, float)
		{
		auto view{scene.ecs_registry.view<iige::ecs::components::transform, iige::ecs::components::speed>()};

		view.each([&](const iige::ecs::components::transform& t, iige::ecs::components::speed& s)
			{
			if (t.position.x < 0                            && s.position.x < 0) { s.position.x *= -1; }
			if (t.position.y < 0                            && s.position.y < 0) { s.position.y *= -1; }
			if (t.position.x > window.sf_window.getSize().x && s.position.x > 0) { s.position.x *= -1; }
			if (t.position.y > window.sf_window.getSize().y && s.position.y > 0) { s.position.y *= -1; }
			});
		});
	
	loop.run();
	
	return 0;
	}
