//hashmap.h
#ifndef HASHMAP_H
#define HASHMAP_H

typedef struct hashmap_t hashmap_t;

typedef int (*equals_callback_t)(void*,void*);

typedef int (*hashcode_callback_t)(void*);

hashmap_t* new_map(equals_callback_t equals_callback,hashcode_callback_t hashcode_callback);

hashmap_t* new_map_with_init_capacity(equals_callback_t equals_callback,hashcode_callback_t hashcode_callback, size_t initial_capacity);

void* map_insert(hashmap_t* map, void* key, void* value);

void* map_get(hashmap_t* map, void* key);

int map_contains_key(hashmap_t* map, void* key);

int map_is_empty(hashmap_t* map);

void map_keys(hashmap_t* map, void** key_list);

void map_values(hashmap_t* map, void** value_list);

void* map_remove(hashmap_t* map, void* key);

size_t map_size(hashmap_t* map);

void map_free(hashmap_t* map);

size_t map_capacity(hashmap_t* map);

#endif //HASHMAP_H