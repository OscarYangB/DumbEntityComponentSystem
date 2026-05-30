#include "ecs.h"
#include <iostream>

struct Position {
	float x{};
	float y{};
};

struct Color {
	uint8_t r{};
	uint8_t g{};
	uint8_t b{};
};

int main() {
	auto entity = new_entity();
	add_component(entity, Position{.x = 5.f});
	add_component(entity, Color{.r = 127});

	auto entity_2 = new_entity();
	add_component(entity_2, Position{.x = 6.f});
	add_component(entity_2, Color{.r = 125});

	auto entity_4 = new_entity();
	add_component(entity_4, Position{.x = 100.f});
	add_component(entity_4, Color{.r = 1});

	auto entity_3 = new_entity();
	add_component(entity_3, Position{.x = 10.f});
	add_component(entity_3, Color{.r = 2});

	for (auto [entity, position] : get_entities<Position>()) {
		std::cout << position->x << "\n";
	}

	for (auto [entity, color] : get_entities<Color>()) {
		std::cout << static_cast<int>(color->r) << "\n";
	}

	sort<Position>([](const Position& first, const Position& second) { return first.x > second.x; });

	for (auto [entity, position] : get_entities<Position>()) {
		std::cout << position->x << "\n";
	}

	return 0;
}
