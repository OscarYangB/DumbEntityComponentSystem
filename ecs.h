#pragma once

#include <bitset>
#include <cassert>
#include <cstdint>
#include <array>
#include <vector>
#include <algorithm>

using EntityType = uint16_t;
#define NULL_ENTITY 0
constexpr EntityType MAX_ENTITIES = 1000;

inline std::bitset<MAX_ENTITIES> entity_id_used{};

inline EntityType new_entity() {
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
	inline static std::array<EntityType, MAX_ENTITIES> entity_to_data_index_map;
	inline static EntityType size = 1;

	static EntityType get_entity(EntityType index) {
		return std::get<EntityType>(data[index]);
	}

	static T* get_component(EntityType index) {
		return &std::get<T>(data[index]);
	}

	static bool has_entity(EntityType entity) {
		return entity_to_data_index_map[entity] != 0;
	}

	static T& push_back(EntityType entity, T&& component) {
		data[size] = {entity, component};
		entity_to_data_index_map[entity] = size;
		size++;
		return std::get<T>(data[size - 1]);
	}

	static void remove(EntityType index) {
		entity_to_data_index_map[std::get<EntityType>(data[index])] = 0;
		data[index] = std::move(data[size - 1]);
		size--;
	}

	template<typename Function>
	static void sort(Function&& function) {
		std::sort(data.begin() + 1, data.begin() + size, [function](const Pair& a, const Pair& b) {
			return function(std::get<T>(a), std::get<T>(b));
		});

		entity_to_data_index_map = {};
		for (int i = 1; i < size; i++) {
			entity_to_data_index_map[std::get<EntityType>(data[i])] = i;
		}
	}
};

template<typename T, typename Function>
void sort(Function&& function) {
	ComponentList<T>::sort(std::forward<Function>(function));
}

template<typename T>
T* get_component(EntityType entity) {
	assert(is_valid_entity(entity));
	assert(ComponentList<T>::has_entity(entity));
	return ComponentList<T>::get_component(ComponentList<T>::entity_to_data_index_map[entity]);
}

template<typename T>
T* try_get_component(EntityType entity) {
	assert(is_valid_entity(entity));
	return ComponentList<T>::has_entity(entity) ? ComponentList<T>::get_component(ComponentList<T>::entity_to_data_index_map[entity]) : nullptr;
}


template<typename T>
T& add_component(EntityType entity, T&& component) {
	assert(is_valid_entity(entity));
	assert(!ComponentList<T>::has_entity(entity));
	return ComponentList<T>::push_back(entity, std::forward<T>(component));
}

template<typename T>
void remove_component(EntityType entity) {
	assert(is_valid_entity(entity));
	assert(ComponentList<T>::has_entity(entity));
	ComponentList<T>::remove(ComponentList<T>::entity_to_data_index_map[entity]);
}

template<typename... T>
std::vector<std::tuple<EntityType, T*...>> get_entities() {
	std::vector<EntityType> found_entities{};

	bool first_iteration = true;
	([&found_entities, &first_iteration]() {
		if (first_iteration) {
			found_entities.reserve(ComponentList<T>::size);
			for (EntityType i = 1; i < ComponentList<T>::size; i++) {
				while (!is_valid_entity(ComponentList<T>::get_entity(i)) && i < ComponentList<T>::size) {
					ComponentList<T>::remove(i);
				}
				if (i >= ComponentList<T>::size) {
					break;
				}
				found_entities.emplace_back(ComponentList<T>::get_entity(i));
			}
		}
		first_iteration = false;
	}(), ...);

	found_entities.erase(std::remove_if(found_entities.begin(), found_entities.end(), [&found_entities](EntityType entity) {
		bool found = true;
		found &= ([&entity]() {
			return ComponentList<T>::has_entity(entity);
		}(), ...);
		return !found;
	}), found_entities.end());

	std::vector<std::tuple<EntityType, T*...>> result{};
	result.reserve(found_entities.size());
	for (EntityType entity : found_entities) {
		result.emplace_back(entity, ComponentList<T>::get_component(ComponentList<T>::entity_to_data_index_map[entity])...);
	}

	return result;
}

template<typename T>
struct Singleton {
	static T object;
	inline static bool exists = false;
};

template<typename T>
T* get_singleton() {
	assert(Singleton<T>::exists);
	return &Singleton<T>::object;
}

template<typename T>
T* try_get_singleton() {
	return Singleton<T>::exists ? &Singleton<T>::object : nullptr;
}

template<typename T>
void create_singleton(T&& object) {
	assert(!Singleton<T>::exists);
	Singleton<T>::exists = true;
	Singleton<T>::object = object;
}

template<typename T>
void destroy_singleton() {
	assert(Singleton<T>::exists);
	Singleton<T>::exists = false;
}
