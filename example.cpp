#include "decs.cpp"
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

	auto entity_2 = new_entity();
	add_component(entity_2, Position{.x = 6.f});
	add_component(entity_2, Color{.r = 125});

	for (auto component : get_components<Position>()) {
		std::cout << component->x << "\n";
	}

	for (auto component : get_components<Color>()) {
		std::cout << static_cast<int>(component->r) << "\n";
	}

	std::cout << sizeof(ComponentList<Position>::components) << "\n";
	std::cout << sizeof(ComponentList<Position>::entities) << "\n";
	return 0;
}
