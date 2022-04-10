
#include <deque>
#include <set>
#include <stack>

#include <utils/memory.h>
#include <utils/tracking.h>

#include <utils/math/geometry/polygon.h>
#include <utils/math/geometry/circle.h>
#include <utils/math/geometry/transformations.h>
#include <utils/math/geometry/interactions.h>

#include <SFML/Graphics.hpp>
#include <entt.h>

#include <utils/containers/polymorphic_container.h>

struct indexed
	{
	size_t index{};
	};

template <typename T>
class batch_container
	{
	template <typename T>
	struct indexed_pair : utils::wrapper<T>, public utils::trackable
		{
		template <typename ...Args>
		indexed_pair(Args&&... args) : utils::wrapper<T>{std::forward<Args>(args)...} {}

		size_t index;
		};

	public:
		void remove(const utils::tracking_ptr<indexed_pair<T>>& target) noexcept
			{
			objects_to_be_removed.insert(target->index); 
			}

		template<typename ... Args>
		utils::tracking_ptr<indexed_pair<T>> emplace(Args ... args) noexcept
			{
			return {objects_to_be_added.emplace(std::forward<Args>(args)...)};
			}

		void update() noexcept
			{
			// Objects to be destroyed replace the place of objects to be added in the objects container.
			std::set<size_t>::iterator objects_to_be_removed_it{objects_to_be_removed.begin()};
			size_t delta_size{objects_to_be_removed.size() - objects_to_be_added.size()};
			while (!objects_to_be_added.empty() && objects_to_be_removed_it != objects_to_be_removed.end())
				{
				objects[*objects_to_be_removed_it] = std::move(objects_to_be_added.top());
				objects[*objects_to_be_removed_it].index = *objects_to_be_removed_it;

				objects_to_be_removed_it++;
				objects_to_be_added.pop();
				}
			if (!objects_to_be_added.empty())
				{
				do
					{
					size_t index{objects.size()};
					auto& new_object{objects.emplace_back(std::move(objects_to_be_added.top()))};
					new_object.index = index;

					objects_to_be_added.pop();
					} 
				while (!objects_to_be_added.empty());
				}
			else
				{
				size_t swap_candidate_index{objects.size() - 1}; //TODO check if size is 0
				std::set<size_t>::iterator last_objects_to_be_removed_it{objects_to_be_removed.end()};
				last_objects_to_be_removed_it--;
				while (true)
					{
					while (
						swap_candidate_index > 0
						&& swap_candidate_index == *last_objects_to_be_removed_it
						&& std::prev(objects_to_be_removed_it) != last_objects_to_be_removed_it)
						{
						swap_candidate_index--; last_objects_to_be_removed_it--;
						}
					if (std::prev(objects_to_be_removed_it) == last_objects_to_be_removed_it) { break; }

					objects[*objects_to_be_removed_it] = std::move(objects[swap_candidate_index]);
					objects[*objects_to_be_removed_it].index = *objects_to_be_removed_it;

					swap_candidate_index--;
					objects_to_be_removed_it++;
					}

				objects.resize(objects.size() - (delta_size));
				}
			objects_to_be_removed.clear();
			}
	private:
		std::deque<indexed_pair<T>> objects;
		std::set<size_t> objects_to_be_removed;
		std::stack<indexed_pair<T>> objects_to_be_added;
	};

class Object
	{
	public:
		const entt::entity entity{entt::null};
	private:
	};



template <typename T>
struct indexed_pair : utils::wrapper<T>
	{
	template <typename ...Args>
	indexed_pair(Args&&... args) : utils::wrapper<T>{std::forward<Args>(args)...} {}

	size_t index;
	};




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
	std::uniform_int_distribution<int> distribution{0, 5};
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

		iige::ecs::components::add_movement (scene.ecs_registry, entity, {position, angle}, {speed, rotation});

		int collider_type{distribution(mt)};
		switch (collider_type)
			{
			case 0:
				iige::ecs::components::add_collision(scene.ecs_registry, entity, iige::ecs::components::collider{utils::math::geometry::polygon{{-10, -10}, {-5, 0}, { 10, -10 }, {10, 10}, {0, 0}, {-10, 10}}});
				break;
			case 1:
				iige::ecs::components::add_collision(scene.ecs_registry, entity, iige::ecs::components::collider{utils::math::geometry::convex_polygon{{0, 0}, {100, 50}, {50, 100}}});
				break;
			case 2:
				iige::ecs::components::add_collision(scene.ecs_registry, entity, iige::ecs::components::collider{utils::math::geometry::circle{{0, 0}, 32}});
				break;
			case 3:
				iige::ecs::components::add_collision(scene.ecs_registry, entity, iige::ecs::components::collider{utils::math::geometry::aabb{.ll{-10}, .up{-10}, .rr{10}, .dw{10}}});
				break;
			case 4:
				iige::ecs::components::add_collision(scene.ecs_registry, entity, iige::ecs::components::collider{utils::math::geometry::segment{{-10, 10},{10, -10}}});
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
	
	iige::Loop loop{scene, window, collision, 10.f};


	//Bouncing system
	loop.user_systems.emplace_back([&](iige::Scene& scene, iige::Window& window)
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
