
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






#include "Window.h"
#include "Scene.h"
#include "Loop.h"

#include "ecs/components/bad_draw.h"
#include "ecs/components/speed.h"
#include "ecs/components/transform.h"

int main()
	{
	using namespace utils::angle::literals;

	iige::Window window{iige::Window::create_info{.size{800, 600}, .position{10, 10}, .title{"My window"}}};

	iige::Scene scene;

	for (size_t i = 0; i < 3000; i++)
		{
		auto entity{scene.ecs_registry.create()};
		scene.ecs_registry.emplace<iige::ecs::components::transform>(entity, utils::math::Transform2{.position{500.f, 350.f}, .orientation{}});
		scene.ecs_registry.emplace<iige::ecs::components::speed>(entity, utils::math::Transform2{.position{10.f, 10.f}, .orientation{5_deg}});
		scene.ecs_registry.emplace<iige::ecs::components::bad_draw>(entity, 32.f);
		scene.ecs_registry.emplace<iige::ecs::components::interpolated>(entity);
		scene.ecs_registry.emplace<iige::ecs::components::transform_next>(entity);
		}


	iige::Loop loop{scene, window, 10.f};

	loop.run();

	return 0;
	}

















namespace utmg = utils::math::geometry;
namespace utm = utils::math;

sf::VertexArray to_sf(const utmg::polygon& polygon, const sf::Color& color) noexcept
	{
	sf::VertexArray ret{sf::LineStrip, polygon.get_vertices().size() + 1};
	for (size_t i = 0; i < polygon.get_vertices().size(); i++)
		{
		const auto& vertex{polygon.get_vertices()[i]};
		ret[i].position = {vertex.x, vertex.y};
		ret[i].color = color;
		}
	ret[polygon.get_vertices().size()] = ret[0];

	return ret;
	}
sf::CircleShape to_sf(const utmg::circle& circle, const sf::Color& color) noexcept
	{
	sf::CircleShape s{circle.radius};
	s.setPosition({circle.center.x, circle.center.y});
	s.setFillColor(sf::Color::Transparent);
	s.setOutlineColor(color);
	s.setOutlineThickness(1);
	s.setOrigin(circle.radius, circle.radius);
	return s;
	}
sf::VertexArray to_sf(const utmg::segment& segment, const sf::Color& color) noexcept
	{
	sf::VertexArray ret{sf::LineStrip, 2};

	ret[0].position = {segment.a.x, segment.a.y};
	ret[1].position = {segment.b.x, segment.b.y};
	ret[0].color = color;
	ret[1].color = color;

	return ret;
	}

sf::VertexArray to_sf(const utm::vec2f& point, const sf::Color& color) noexcept
	{
	sf::VertexArray ret{sf::Points, 1};

	ret[0].position = {point.x, point.y};
	ret[0].color = color;

	return ret;
	}

int mainzq()
	{
	using namespace utils::angle::literals;
	using namespace utils::math::geometry::transformations;
	using poly_convex = utmg::convex_polygon;
	using poly_gon    = utmg::polygon;

	sf::RenderWindow rw{{800, 600}, "pippo"};
	rw.setFramerateLimit(60);

	utmg::segment src_segment{{-10, 0}, {10, 0}};
	utm ::vec2f   src_point  {0, 0};
	poly_convex   src_poly   {{{-10, -10}, {10, -10}, {10, 10},         {-10, 10}}};
	poly_gon      src_cttv   {{{-10, -10}, {10, -10}, {10, 10}, {0, 0}, {-10, 10}}};
	utmg::circle  src_circle {{0, 0}, 10};

	utm::Transform2	tr_segment{{200, 300},  58_deg, 1.f};
	utm::Transform2	tr_point  {{100, 400},  24_deg, 1.f};
	utm::Transform2	tr_poly   {{250, 300}, 105_deg, 1.f};
	utm::Transform2	tr_cttv   {{250, 300}, 105_deg, 1.f};
	utm::Transform2	tr_circle {{350, 400},  75_deg, 1.f};

	utmg::segment segment{src_segment * tr_segment};
	utm ::vec2f   point  {src_point   * tr_point  };
	poly_convex   poly   {src_poly    * tr_poly   };
	poly_gon      cttv   {src_cttv    * tr_cttv   };
	utmg::circle  circle {src_circle  * tr_circle };

	utils::observer_ptr<utm::Transform2> selected_transform{nullptr};


	auto side{segment.point_side(point)};

	while (rw.isOpen())
		{

		//input
		sf::Event event;
		while (rw.pollEvent(event))
			{
			switch (event.type)
				{
				case sf::Event::Closed: rw.close(); break;

				case sf::Event::KeyPressed:
					if (true)
						{
						switch (event.key.code)
							{
							case sf::Keyboard::Num1: selected_transform = &tr_segment; break;
							case sf::Keyboard::Num2: selected_transform = &tr_point;   break;
							case sf::Keyboard::Num3: selected_transform = &tr_poly;    break;
							case sf::Keyboard::Num4: selected_transform = &tr_cttv;    break;
							case sf::Keyboard::Num5: selected_transform = &tr_circle;  break;
							}

						if (selected_transform != nullptr)
							{
							switch (event.key.code)
								{
								case sf::Keyboard::Right: selected_transform->position.x++; break;
								case sf::Keyboard::Left:  selected_transform->position.x--; break;
								case sf::Keyboard::Up:    selected_transform->position.y--; break;
								case sf::Keyboard::Down:  selected_transform->position.y++; break;

								case sf::Keyboard::PageUp:   selected_transform->orientation -= 1_deg; break;
								case sf::Keyboard::PageDown: selected_transform->orientation += 1_deg; break;

								case sf::Keyboard::Add:      selected_transform->size *= 2; break;
								case sf::Keyboard::Subtract: selected_transform->size /= 2; break;
								}
							}
						segment = {src_segment * tr_segment};
						point   = {src_point   * tr_point  };
						poly    = {src_poly    * tr_poly   };
						cttv    = {src_cttv    * tr_cttv   };
						circle  = {src_circle  * tr_circle };
						}
				}
			}
		//step
		bool st{utmg::collides(segment, point)};
		bool sp{utmg::collides(segment, poly)};
		bool sv{utmg::collides(segment, cttv)};
		bool tp{utmg::collides(point,   poly)};
		bool tv{utmg::collides(point,   cttv)};
		bool pc{utmg::collides(poly,    circle)};
		bool vc{utmg::collides(cttv,    circle)};
		bool tc{utmg::collides(point,   circle)};
		bool cs{utmg::collides(circle,  segment)};
		bool pv{utmg::collides(poly,    cttv)};



		//draw
		rw.clear();

		rw.draw(to_sf(segment, (st || sp || cs || sv) ? sf::Color::Red : sf::Color::White));
		rw.draw(to_sf(point,   (tp || tc || st || tv) ? sf::Color::Red : sf::Color::White));
		rw.draw(to_sf(poly,    (tp || sp || pc || pv) ? sf::Color::Red : sf::Color::White));
		rw.draw(to_sf(circle,  (pc || tc || cs || vc) ? sf::Color::Red : sf::Color::White));
		rw.draw(to_sf(cttv,    (pv || vc || tv || sv) ? sf::Color::Red : sf::Color::White));

		rw.display();
		}
	return 0;
	}
	