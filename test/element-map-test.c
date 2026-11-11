#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../src/element-map.h"

int main(int argc, char** argv) {
    // Init a new map
    map_t element_map = MAP_INIT;
    printf("Initialised element map\n");

    int result;

    // Add elements
    const char* new_strings[] = {"new_element", "cool_element", "fire_element", "nice_element", "shitty_element"};
    size_t str_amount = 5;

    for(int i = 0; i < str_amount; i++) {
        int id = rand() % 100 + 1;
        result = add_element_map(&element_map, new_strings[i], strlen(new_strings[i]), id);
        printf("Added element %d [%s -> %d] - Result: %d\n", i, new_strings[i], id, result);
    }

    char* search = "new_element";
    int found_id;
    result = get_element_map(&element_map, search, strlen(search), &found_id);
    printf("Get element '%s' returned %d -> %d\n", search, result, found_id);

    // Noid set
    const char* new_noid_str[] = {"i", "do", "not", "like", "ids"};
    size_t noid_str_len = 5;

    for(int i = 0; i < noid_str_len; i++) {
        unsigned int assigned_id;
        result = add_element_map_noid(&element_map, new_noid_str[i], strlen(new_noid_str[i]), &assigned_id);
        printf("Added noid element %d [%s -> %d] - Result: %d\n", i, new_noid_str[i], assigned_id, result);
    }
    
    search = "not";
    result = get_element_map(&element_map, search, strlen(search), &found_id);
    printf("Get element '%s' returned %d -> %d\n", search, result, found_id);

    // Set new values
    for(int i = 0; i < str_amount; i++) {
        int id = rand() % 100 + 1;
        result = add_element_map(&element_map, new_strings[i], strlen(new_strings[i]), id);
        printf("Added element %d [%s -> %d] - Result: %d\n", i, new_strings[i], id, result);
    }
    
    search = "fire_element";
    result = get_element_map(&element_map, search, strlen(search), &found_id);
    printf("Get element '%s' returned %d -> %d\n", search, result, found_id);

    // Assignment of other strings with no id
    const char* other_noid_strs[] = {"want", "to", "make", "more", "elements"};
    size_t other_noid_len = 5;
    
    for(int i = 0; i < noid_str_len; i++) {
        unsigned int assigned_id;
        result = add_element_map_noid(&element_map, other_noid_strs[i], strlen(other_noid_strs[i]), &assigned_id);
        printf("Added noid element %d [%s -> %d] - Result: %d\n", i, other_noid_strs[i], assigned_id, result);
    }
    
    // Search
    search = "make";
    result = get_element_map(&element_map, search, strlen(search), &found_id);
    printf("Get element '%s' returned %d -> %d\n", search, result, found_id);

    clear_element_map(&element_map);
}