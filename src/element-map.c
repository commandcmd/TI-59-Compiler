#include "element-map.h"
#include "global-helpers.h"

/*
 * Two constants used in the FNV-1a computation.
 */
const uint64_t FNV_offset_basis = 0xcbf29ce484222325;
const uint64_t FNV_prime = 0x100000001b3;

/*
 * This function computes the FNV-1a hash
 * for the given string in a 64 bit output
 * for fast memory lookup.
 */
uint64_t compute_hash(const char* name, size_t size) {
    // Initialise the hash output
    uint64_t hash = FNV_offset_basis;

    // For each byte of data
    for(size_t i = 0; i < size; i++) {
        hash = hash ^ (uint8_t)name[i]; // XOR the hash with the byte of data
        hash *= FNV_prime; // Multiply with the prime
    }

    return hash;
}

/*
 * Getter for the element map which
 * doesn't compute the hash of the
 * given string, to allow optimizations
 * where the hash of a string is precomputed
 * instead of being computed twice.
 */
int _get_element_map(map_t* map, const char* name, size_t length, int* retrieved_id, uint64_t element_hash, unsigned int* found_position) {
    // Iterate through all the strings in the string array
    for(unsigned int current_str = 0; current_str < map->size; current_str++) {
        // Check if the hashes match
        if(map->map[current_str].hash == element_hash) {
            // If the hashes match, then compare the actual strings
            if(nnt_strcmp(map->map[current_str].element, name, length)) {
                // Found match
                if(retrieved_id != NULL)
                    *retrieved_id = map->map[current_str].id;
                if(found_position != NULL)
                    *found_position = current_str;
                return 1;
            }
        }
    }

    // No strings found
    return 0;
}

/*
 * Add element function which assigns to the element
 * the provided id and computes a hashmap for speed.
 * In case the assigned_id isn't set to NULL, then 
 * the value contained in provided_id is discarded
 * and an ID is assigned.
 *
 * This function checks if the map already contains
 * the given element, in which case it just updates
 * the id of the existing element.
 */
int _add_element_map(map_t* map, const char* name, size_t size, int provided_id, int* assigned_id) {
    // First, check if the element requested exists already
    unsigned int found_position;
    if(size < 1) return -1;
    // Precompute the element's hash
    uint64_t element_hash = compute_hash(name, size);
    if(_get_element_map(map, name, size, NULL, element_hash, &found_position)) {
        // If the element exists already update its value
        if(assigned_id != NULL) {
            *assigned_id = found_position;
            map->map[found_position].id = found_position;
        } else map->map[found_position].id = provided_id;
        // Successfully updated
        return 0;
    }

    // Reallocate the string array, and allocate it if needed
    if(map->map == NULL || map->size == 0) {
        map->map = (map_element*)malloc(sizeof(map_element));
        if(map->map == NULL) return -2;
        map->size = 1;
    } else {
        map_element* new_map = (map_element*)realloc(map->map, ++(map->size) * sizeof(map_element));
        if(new_map == NULL) return -1;
        map->map = new_map;
    }

    // Allocate a separate memory block for the string
    char* new_map = (char*)malloc(size + 1);
    memcpy(new_map, name, size);
    new_map[size] = '\0'; // Null-terminate the string
    if(new_map == NULL) return -3;
    map->map[map->size - 1].element = new_map;
    // Assign an id if needed, otherwise use the provided one
    if(assigned_id != NULL) {
        // The assigned ID is just the location of the object
        *assigned_id = map->size - 1;
        map->map[map->size - 1].id = *assigned_id;
    } else {
        // Assign the ID to this element map
        map->map[map->size - 1].id = provided_id;
    }
    // Save the precomputed hash in this object
    map->map[map->size - 1].hash = element_hash;
    return 0;
}

int add_element_map(map_t* map, const char* name, size_t length, int provided_id) {
    return _add_element_map(map, name, length, provided_id, NULL);
}

int add_element_map_noid(map_t* map, const char* name, size_t length, int* assigned_id) {
    if(assigned_id == NULL) return -1;
    return _add_element_map(map, name, length, 0, assigned_id);
}

int get_element_map(map_t* map, const char* name, size_t length, int* retrieved_id) {
    // Compute and save the has of the input
    uint64_t input_hash = compute_hash(name, length);
    // Run the internal get element map function
    return _get_element_map(map, name, length, retrieved_id, input_hash, NULL);
}

void clear_element_map(map_t* map) {
    if(map->map == NULL) return; // Don't free anything if no array is available

    // Iterate through all the program pointers
    for(int i = 0; i < map->size; i++)
        free(map->map[i].element);
    // Free the entire progpointers array
    free(map->map);
}