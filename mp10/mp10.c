#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "mp10.h"

int32_t 
mark_vertex_minimap (graph_t* g, pyr_tree_t* p)
{
    //If the graph has 64 vertices or fewer, simply use each vertex’ array index
    if(g->n_vertices <= 64){
        for(int32_t i=0; i< g->n_vertices; i++){
            g->vertex[i].mm_bit = i;
        }
    }
    else{
        // do for each vertex in the graph
        for(int32_t graph_index = 0;graph_index < g->n_vertices;graph_index++){
            int32_t node_num = 64;    // start from the fourth layer
            int32_t current_total_nodes = 85;
            while(current_total_nodes < p->n_nodes){
                node_num *= 4;
                current_total_nodes += node_num;
            }
            current_total_nodes -= node_num;   
            for(int32_t i = current_total_nodes;i < p->n_nodes; i++){
                if(p->node[i].id == graph_index){
                    while(i>84){
                        i = (i-1)/4; //find the parent node
                    }
                    g->vertex[graph_index].mm_bit=i-21;
                    break;
                }
            }
        }
    }
    return 1;
}


void 
build_vertex_set_minimap (graph_t* g, vertex_set_t* vs)
{
    uint64_t bits = 0;
    for(int32_t i=0;i<vs->count;i++){
        bits = bits | (1ULL << g->vertex[vs->id[i]].mm_bit);
    }
    vs->minimap = bits;

}


void 
build_path_minimap (graph_t* g, path_t* p)
{
    uint64_t bits = 0;
    for(int32_t i=0;i<p->n_vertices;i++){
        bits = bits | (1ULL << g->vertex[p->id[i]].mm_bit);
    }
    p->minimap = bits;
}


int32_t
merge_vertex_sets (const vertex_set_t* v1, const vertex_set_t* v2,
		   vertex_set_t* vint)
{
    vint->count = 0;
    for(int32_t i=0;i < v1->count;i++){
        for(int32_t j=0; j < v2->count; j++){
            if(v1->id[i]==v2->id[j]){
                vint->id[vint->count] = v1->id[i];
                vint->count++;
                break;  // match found, stop searching v2 and go back to v1
            }
        }
    }
    if(vint->count) return 1;
    return 0;
}

