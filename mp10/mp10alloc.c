#include <stdint.h>
#include <stdlib.h>

#include "mp10.h"


vertex_set_t*
new_vertex_set ()
{
    vertex_set_t* ptr;
    ptr = malloc(sizeof(vertex_set_t));
    if(ptr == NULL)     return NULL;
    ptr->count = 0;
    ptr->id = malloc(2*sizeof(*ptr->id));
    if(ptr->id==NULL){
        free(ptr);
        return NULL;
    } 
    ptr->id_array_size = 2;
    return ptr;
}


void
free_vertex_set (vertex_set_t* vs)
{
    if(vs->id != NULL)  free(vs->id);
    free(vs);
}


path_t*
new_path ()
{
    path_t* ptr;
    ptr = malloc(sizeof(path_t));
    if(ptr == NULL)     return NULL;
    ptr->id = NULL;
    return ptr;
}


void
free_path (path_t* path)
{
    if(path->id != NULL)  free(path->id);
    free(path);
}

