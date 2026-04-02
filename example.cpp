#include "decs.cpp"
#include <iostream>

struct Position {
	float x{};
	float y{};
};

int main() {
	auto entity = new_entity();
	add_component(entity, Position{.x = 5.f});

	auto entity_2 = new_entity();
	add_component(entity_2, Position{.x = 6.f});

	for (auto component : get_components<Position>()) {
		std::cout << component->x << "\n";
	}

	std::cout << sizeof(ComponentList<Position>::components) << "\n";
	std::cout << sizeof(ComponentList<Position>::entities) << "\n";
	return 0;
}
