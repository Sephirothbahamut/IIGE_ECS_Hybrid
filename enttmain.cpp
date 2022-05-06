#include <iostream>
#include <iomanip>
#include "ecs/entt.h"

struct empty {};
struct stuff { uint64_t x; };

struct parent
	{
	size_t children_count{0};
	entt::entity first_child{entt::null};
	};
struct child
	{
	entt::entity parent{entt::null};
	entt::entity next_sibling{entt::null};
	entt::entity prev_sibling{entt::null};
	};

int mainq()
	{
	try
		{
		entt::registry registry;

		auto a{registry.create()};
		auto b{registry.create()};
		auto c{registry.create()};
		auto d{registry.create()};
		auto e{registry.create()};
		auto f{registry.create()};
		auto g{registry.create()};

		//registry.emplace<child>(a);
		registry.emplace<child>(c, a, entt::null, b);
		registry.emplace<child>(b, a, c, entt::null);

		registry.emplace<child>(d, c);

		registry.emplace<child>(g, d);
		registry.emplace<child>(f, g, e, entt::null);
		registry.emplace<child>(e, g, entt::null, f);

		registry.emplace<stuff>(a, uint64_t{      1});
		registry.emplace<stuff>(b, uint64_t{     10});
		registry.emplace<stuff>(c, uint64_t{    100});
		registry.emplace<stuff>(d, uint64_t{   1000});
		registry.emplace<stuff>(e, uint64_t{  10000});
		registry.emplace<stuff>(f, uint64_t{ 100000});
		registry.emplace<stuff>(g, uint64_t{1000000});

		registry.sort<child>([&registry](const entt::entity lhs, const entt::entity rhs) {
			const auto& clhs = registry.get<child>(lhs);
			const auto& crhs = registry.get<child>(rhs);
			return crhs.parent == lhs || clhs.next_sibling == rhs
				|| (!(clhs.parent == rhs || crhs.next_sibling == lhs) && (clhs.parent < crhs.parent || (clhs.parent == crhs.parent && &clhs < &crhs)));
			});

		auto parentless{registry.view<stuff>(entt::exclude<child>)};
		auto children{registry.view<child, stuff>()};

		parentless.each([](entt::entity entity, stuff& stuff)
			{
			stuff.x *= 2;
			});
		children.each([&](entt::entity entity, const child& child_data, stuff& my_stuff)
			{
			if (child_data.parent != entt::null) { my_stuff.x += registry.get<stuff>(child_data.parent).x; }
			});

		auto all{registry.view<stuff>()};
		all.each([](entt::entity entity, stuff& stuff)
			{
			std::cout << std::setw(3) << static_cast<char>('a' + static_cast<uint32_t>(entity)) << std::setw(20) << stuff.x << "\n";
			});
		}
	catch (const std::exception& e) { std::cout << e.what(); }
	return 0;
	}