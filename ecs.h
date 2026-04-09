#pragma once

#include <bitset>
#include <cassert>
#include <cstdint>
#include <array>
#include <vector>

using EntityType = uint16_t;
#define NULL_ENTITY 0
constexpr EntityType MAX_ENTITIES = std::numeric_limits<EntityType>::max();

inline std::bitset<MAX_ENTITIES> entity_id_used{};

inline uint64_t new_entity() {
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

inline bool is_valid_entity(EntityType entity) {
	return entity_id_used[entity];
}

inline void delete_entity(EntityType entity) {
	assert(is_valid_entity(entity));
	entity_id_used[entity] = false;
}

template<typename T>
struct ComponentList {
	using Pair = std::tuple<EntityType, T>;
	inline static std::array<Pair, MAX_ENTITIES> data;
	inline static std::array<T*, MAX_ENTITIES> entity_component_map;
	inline static EntityType size = 1;

	static EntityType get_entity(EntityType index) {
		return std::get<EntityType>(data[index]);
	}

	static T& push_back(EntityType entity, T&& component) {
		data[size] = {entity, component};
		entity_component_map[entity] = &std::get<T>(data[size]);
		size++;
		return std::get<T>(data[size - 1]);
	}

	static void remove(EntityType index) {
		entity_component_map[std::get<EntityType>(data[index])] = nullptr;
		data[index] = std::move(data[size - 1]);
		size--;
	}

	template<typename Function>
	static void sort(Function&& function) { // TODO Bugged!!
		std::sort(data.begin(), data.begin() + size, [function](const Pair& a, const Pair& b) { return function(std::get<T>(a), std::get<T>(b)); });
		entity_component_map = {};
		for (int i = 1; i < size; i++) {
			entity_component_map[std::get<EntityType>(data[i])] = &std::get<T>(data[i]);
		}
	}
};

template<typename T, typename Function>
void sort(Function&& function) {
	ComponentList<T>::sort(function);
}

template<typename T>
T* get_component(EntityType entity) {
	assert(is_valid_entity(entity));
	return ComponentList<T>::entity_component_map[entity];
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
		if (ComponentList<T>::get_entity(i) == entity) {
			ComponentList<T>::remove(i);
			return;
		}
	}
}

template<typename... T>
std::vector<std::tuple<EntityType, T*...>> get_entities() {
	std::bitset<MAX_ENTITIES> found_entities{};
	found_entities.set();

	([&found_entities](){ // TODO Can optimize this by checking the entity component map for nullptr?
		std::bitset<MAX_ENTITIES> current_found_entities{};
		for (EntityType i = 1; i < ComponentList<T>::size; i++) {
			if (!is_valid_entity(ComponentList<T>::get_entity(i))) {
				ComponentList<T>::remove(i);
				if (i >= ComponentList<T>::size) break;
			}
			current_found_entities[ComponentList<T>::get_entity(i)] = true;
		}
		found_entities &= current_found_entities;
	}(), ...);

	std::vector<std::tuple<EntityType, T*...>> result{};
	for (EntityType i = 0; i < found_entities.size(); i++) {
		if (found_entities[i]) {
			result.push_back({i, get_component<T>(i) ...});
		}
	}

	return result;
}
