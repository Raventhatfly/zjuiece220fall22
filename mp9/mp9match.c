#include <stdint.h>
#include <stdio.h>

#include "mp5.h"
#include "mp9.h"

/*
Author: Wu Feiyang
Finish Date: 29th Nov, 2022
Introductory Paragraph
This file contains the function match_requests which takes in 
several arguments, graph, pyramid tree, heap, two requests,
source vertices, destination vertices and path.
The function first find the destination and source vertices
by find_nodes and trim_nodes, then apply the dijkstra algorithm.

*/


/*
 * match_requests -- find the souce vertices and the destination vertices
 *					 and then implementing the dijkstra algorithm. The ideal
 *					 path will be written to the struct path.
 * INPUTS: g -- graph pointer
 * 		   p -- pyramid tree pointer
 *         h -- heap pointer
 * 		   r1 -- first request pointer
 * 		   r2 -- second request pointer 
 *         src -- source vertex set pointer
 *         dest -- destination vertex set pointer
 *         path -- path struct pointer
 * OUTPUTS: path -- ideal path written into struct path
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */
int32_t
match_requests (graph_t* g, pyr_tree_t* p, heap_t* h,
		request_t* r1, request_t* r2,
		vertex_set_t* src_vs, vertex_set_t* dst_vs, path_t* path)
{	
	int32_t r=0;	//return value
	src_vs->count = 0;
	dst_vs->count = 0;
	// find souce vertices
	find_nodes(&r1->from,src_vs,p,0);
	trim_nodes(g,src_vs,&r2->from);
	// find destination vertices
	find_nodes(&r1->to,dst_vs,p,0);
	trim_nodes(g,dst_vs,&r2->to);
	// make sure that vertex set is not empty before applying dijkstra
	if(src_vs->count && dst_vs->count)	r = dijkstra(g,h,src_vs,dst_vs,path);
	return r;
}
