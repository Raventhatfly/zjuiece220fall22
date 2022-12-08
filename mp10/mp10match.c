#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "mp5.h"
#include "mp10.h"

#define MATCH_SIZE 500


//
// These variables hold the heads of two singly-linked lists of 
// requests.  
//
// The avaialble list consists of unpaired requests: partner 
// should be NULL, and next is used to form the list.   
// 
// The shared list consists of groups with non-empty start and end 
// vertex intersections.  Only one of the requests in the group is in 
// the list.  The others are linked through the first's partner field 
// and then through the next field of the others in the group.  The 
// next field of the first request in a group is used to form the
// shared list.
//
// Note: for MP2, you should only build groups of two in the shared
// list.
//

static request_t* available = NULL;
static request_t* shared = NULL;


int32_t
handle_request (graph_t* g, pyr_tree_t* p, heap_t* h, request_t* r)
{
    vertex_set_t* src_vs = new_vertex_set();
    vertex_set_t* dst_vs = new_vertex_set();
    vertex_set_t* match_src_vs = new_vertex_set();
    vertex_set_t* match_dst_vs = new_vertex_set();
    path_t* path = new_path();
    if(src_vs && dst_vs && path && match_src_vs && match_dst_vs == NULL ){
        if(src_vs != NULL)    free_vertex_set(src_vs);
        if(dst_vs != NULL)    free_vertex_set(dst_vs);
        if(match_src_vs != NULL)    free_vertex_set(match_src_vs);
        if(match_dst_vs != NULL)    free_vertex_set(match_dst_vs);
        if(path != NULL)      free_path(path);
        return 0;
    }
    find_nodes(&r->from,src_vs,p,0);
    find_nodes(&r->to,dst_vs,p,0);
    // update the vertex set of the request
    r->src_vs = src_vs;
    r->dst_vs = dst_vs;
    if(src_vs->count==0 || dst_vs->count ==0){
        free_vertex_set(src_vs);
        free_vertex_set(dst_vs);
        free_vertex_set(match_src_vs);
        free_vertex_set(match_dst_vs);
        free_path(path);
        return 0;
    }   

    mark_vertex_minimap(g,p);
    build_vertex_set_minimap(g,src_vs);
    build_vertex_set_minimap(g,dst_vs);

    int32_t* temp_ptr1;
    int32_t* temp_ptr2;
    temp_ptr1 = realloc(match_src_vs->id,MATCH_SIZE);    // make it large enough
    if(temp_ptr1 == NULL) return 0;
    temp_ptr2 = realloc(match_dst_vs->id,MATCH_SIZE);
    if(temp_ptr2 == NULL){
        free(temp_ptr1);
        return 0;
    }
    match_src_vs->id = temp_ptr1;
    match_dst_vs->id = temp_ptr2;
    match_dst_vs->id_array_size = MATCH_SIZE;
    match_dst_vs->id_array_size = MATCH_SIZE;
    
    if(available == NULL){
        available = r; // if available empty add this request into available
        available->next = NULL;
        return 1;      // return on success
    }     
    request_t* cur_reqeust = available;
    request_t* prev_request = NULL;
    request_t* share_ptr;
    while(cur_reqeust != NULL){
        if(((cur_reqeust->src_vs->minimap | src_vs->minimap)!=0 )||
            (cur_reqeust->dst_vs->minimap | dst_vs->minimap)!=0
        ){
            merge_vertex_sets(cur_reqeust->src_vs,src_vs,match_src_vs);
            merge_vertex_sets(cur_reqeust->dst_vs,dst_vs,match_dst_vs);
            if(match_src_vs->count != 0 && match_dst_vs->count != 0){   //if both are nonempty
                dijkstra(g,h,match_src_vs,match_dst_vs,path);
                // pair the two requests
                cur_reqeust->src_vs = match_src_vs;
                cur_reqeust->dst_vs = match_dst_vs;
                //
                r->src_vs = match_src_vs;
                r->dst_vs = match_dst_vs;
                // free both requests vertex sets
                free(src_vs);
                free(dst_vs);
                free(cur_reqeust->src_vs);
                free(cur_reqeust->dst_vs);
                // fill in the path
                cur_reqeust->path = path;
                r->path = path;
                // 
                cur_reqeust->partner = NULL;
                cur_reqeust->next = NULL;
                r->partner = cur_reqeust;
                if(prev_request != NULL){   // remove the current request from the available list
                    prev_request->next = cur_reqeust->next;
                }
                // insert the pair to the end of the list shared
                // if share is empty, just put it in
                if(share_ptr == NULL){
                    share_ptr = r;
                    share_ptr->next = NULL;
                }else{
                    for(share_ptr = shared;share_ptr->next != NULL; share_ptr = share_ptr->next);
                    share_ptr->next = r;
                    r->next = NULL;
                    return 1;  //request found, end the program
                }
                
            }
        }
        prev_request = cur_reqeust;
        cur_reqeust = cur_reqeust->next;
    }
    // if end of the list and do not find a match, insert it into available
    prev_request -> next = r;
    r -> next = NULL;
    return 1;
}


void
print_results ()
{
    request_t* r;
    request_t* prt;

    printf ("Matched requests:\n");
    for (r = shared; NULL != r; r = r->next) {
        printf ("%5d", r->uid);
	for (prt = r->partner; NULL != prt; prt = prt->next) {
	    printf (" %5d", prt->uid);
	}
	printf (" src=%016lX dst=%016lX path=%016lX\n", r->src_vs->minimap,
		r->dst_vs->minimap, r->path->minimap);
    }

    printf ("\nUnmatched requests:\n");
    for (r = available; NULL != r; r = r->next) {
        printf ("%5d src=%016lX dst=%016lX\n", r->uid, r->src_vs->minimap,
		r->dst_vs->minimap);
    }
}


int32_t
show_results_for (graph_t* g, int32_t which)
{
    request_t* r;
    request_t* prt;

    // Can only illustrate one partner.
    for (r = shared; NULL != r; r = r->next) {
	if (which == r->uid) {
	    return show_find_results (g, r, r->partner);
	}
	for (prt = r->partner; NULL != prt; prt = prt->next) {
	    if (which == prt->uid) {
		return show_find_results (g, prt, r);
	    }
	}
    }

    for (r = available; NULL != r; r = r->next) {
        if (which == r->uid) {
	    return show_find_results (g, r, r);
	}
    }
    return 0;
}


static void
free_request (request_t* r)
{
    free_vertex_set (r->src_vs);
    free_vertex_set (r->dst_vs);
    if (NULL != r->path) {
	free_path (r->path);
    }
    free (r);
}

void
free_all_data ()
{
    request_t* r;
    request_t* prt;
    request_t* next;

    // All requests in a group share source and destination vertex sets
    // as well as a path, so we need free those elements only once.
    for (r = shared; NULL != r; r = next) {
	for (prt = r->partner; NULL != prt; prt = next) {
	    next = prt->next;
	    free (prt);
	}
	next = r->next;
	free_request (r);
    }

    for (r = available; NULL != r; r = next) {
	next = r->next;
	free_request (r);
    }
}


