#pragma once

#include <bitset>
#include <cassert>
#include <cstdint>
#include <array>
#include <vector>

using EntityType = uint16_t;
#define NULL_ENTITY 0
constexpr size_t MAX_ENTITIES = std::numeric_limits<EntityType>::max();

//#ifdef DECS_IMPLEMENTATION
std::bitset<MAX_ENTITIES> entity_id_used{};

uint64_t new_entity() {
	EntityType id = NULL_ENTITY;
	for (EntityType i = 1; i < entity_id_used.size(); i++) {
		if (!entity_id_used[i]) {
			id = i;
			entity_id_used[i].flip();
			break;
		}
	}
	assert(id != NULL_ENTITY);
	return id;
}

bool is_valid_entity(EntityType entity) {
	return entity_id_used[entity];
}

void delete_entity(EntityType entity) {
	assert(is_valid_entity(entity));
	entity_id_used[entity] = false;
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

	static void remove(EntityType index) {
		components[index] = components[size - 1];
		entities[index] = entities[size - 1];
		size--;
	}
};

template<typename T>
T* get(EntityType entity) {
	assert(is_valid_entity(entity));
	for (EntityType i = 0; i < ComponentList<T>::size; i++) {
		if (ComponentList<T>::entities[i] == entity) {
			return &ComponentList<T>::components[i];
		}
	}

	return nullptr;
}

template<typename T>
T& add_component(EntityType entity, T&& component) {
	assert(is_valid_entity(entity));
	return ComponentList<T>::push_back(entity, std::forward<T>(component));
}

template<typename T>
void remove_component(EntityType entity) {
	assert(is_valid_entity(entity));
	for (EntityType i = 1; i < ComponentList<T>::size; i++) {
		if (ComponentList<T>::entities[i] == entity) {
			ComponentList<T>::remove(i);
			return;
		}
	}
}

template<typename T> // TODO Use fold expressions to iterate upon variadic templates
std::vector<T*> get_components() {
	std::vector<T*> result{};
	for (EntityType i = 1; i < ComponentList<T>::size; i++) {
		if (!is_valid_entity(ComponentList<T>::entities[i])) {
			ComponentList<T>::remove(i);
			if (i >= ComponentList<T>::size) break;
		}
		result.push_back(&ComponentList<T>::components[i]);
	}
	return result;
}
