#ifndef ELEMENT_MAP_H
#define ELEMENT_MAP_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "ti-error.h"

/* Macro used for map_t initialisation 
 * This is defined for static usage at the start
 * of a module's definition, such as:
 * map_t global_map = MAP_INIT;
*/
#define MAP_INIT {.map = NULL, .size = 0}

/*
 * This structure contains the map of a string
 * (which formally is the element this module
 *  is mapping) to the id that it maps to.
 */
typedef struct map_element {
    char* element;
    uint64_t hash;
    int id;
} map_element;
// Type definition for the map type
typedef struct map_str {
    map_element* map;
    size_t size;
} map_t;

/*
 * This function adds a new element to the map
 * with a provided ID.
 * The element is a tuple which associates an integer
 * value to a string, if the string passed is already
 * contained in the map, the ID is updated with the 
 * value given.
 *
 * NOTE: This function uses dynamic memory allocation, make
 *          sure to ALWAYS call clear_element_map() when
 *          the map is not being used anymore.
*/
int add_element_map(map_t* map, const char* name, size_t name_length, int id);
/*
 * This function adds a new element to the map
 * and assigns to it an incremental ID.
 * The element is a tuple which associates an integer
 * value to a string, if the string passed is already
 * contained in the map, this function works like a get
 * instruction, which returns the ID that element had.
 * It is guaranteed for the assigned ID to be incremental.
 * on a single map ids are either assigned or manually set,
 * letting this function assign an id to an element on a 
 * map which has elements with already set ids will break
 * the guarantee that this code gives for the assigned
 * ids in a map to be incremental.
 *
 * NOTE: This function uses dynamic memory allocation, make
 *          sure to ALWAYS call clear_element_map() when
 *          the map is not being used anymore.
*/
int add_element_map_noid(map_t* map, const char* name, size_t name_length, int* assigned_id);
/*
 * Fast getter for an element in the map, it searches
 * the given string and returns its id through the
 * passed pointer. 
 * This function is optimized and uses a hash lookup
 * for the retrieve operation.
 */
int get_element_map(map_t* map, const char* name, size_t name_length, int* found_id);
/*
 * Clears all the dynamic memory allocations done in a map
 * for each element, and then clears the memory allocation 
 * for the map itself.
 * After calling this function the map object can be safely
 * destroyed.
 */
void clear_element_map(map_t* map);

#endif