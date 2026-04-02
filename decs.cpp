#pragma once

#include <cstdint>
#include <vector>
#include <array>

using EntityType = uint16_t;
constexpr size_t MAX_ENTITIES = std::numeric_limits<EntityType>::max();

//#ifdef DECS_IMPLEMENTATION
EntityType counter = 0;
std::vector<EntityType> entities{};

uint64_t new_entity() {
	auto id = counter;
	entities.push_back(id);
	if (counter == MAX_ENTITIES) {
		counter = 0;
	} else {
		counter++;
	}
	return id;
}
//#endif

template<typename T>
struct ComponentList {
	inline static std::array<EntityType, MAX_ENTITIES> entities{};
	inline static std::array<T, MAX_ENTITIES> components{};
	inline static EntityType size = 0;

	static T& push_back(EntityType entity, T&& component) {
		components[size] = component;
		entities[size] = entity;
		size++;
		return components[size - 1];
	}
};

template<typename T>
T* get(EntityType entity) {
	for (EntityType i = 0; i < ComponentList<T>::size; i++) {
		if (ComponentList<T>::entities[i] == entity) {
			return &ComponentList<T>::components[i];
		}
	}

	return nullptr;
}

template<typename T>
T& add_component(EntityType entity, T&& component) {
	return ComponentList<T>::push_back(entity, std::forward<T>(component));
}

template<typename T> // TODO Use fold expressions to iterate upon variadic templates
std::vector<T*> get_components() {
	std::vector<T*> result{};
	for (EntityType i = 0; i < ComponentList<T>::size; i++) {
		result.push_back(&ComponentList<T>::components[i]);
	}
	return result;
}

template<typename T>
void remove_component(EntityType entity) {
	for (EntityType i = 0; i < ComponentList<T>::size; i++) {
		if (ComponentList<T>::entities[i] == entity) {
			ComponentList<T>::components[i] = ComponentList<T>::components[ComponentList<T>::size - 1];
			ComponentList<T>::entities[i] = ComponentList<T>::entities[ComponentList<T>::size - 1];
			ComponentList<T>::size--;
		}
	}
}

template<typename T>
void delete_entity(EntityType entity) {
	// TODO
}
