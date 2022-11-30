#include <stdint.h>
#include <stdio.h>

#include "mp5.h"
#include "mp9.h"

#define MY_INFINITY   1000000000
#define UNDEFINED    -1             // undefine id marker

/*
Author: Wu Feiyang
Finish Date: 29th Nov, 2022
Introductory Paragraph
This file contains several functions used to find nodes, the heap related 
functions and dijkstra algorithm function.
The find_functions find the nodes within one set, and trim nodes clear the
invalid vertices in the graph. The dijkstra algorithm uses an empty heap. 
In this file I wrote serveral functions implemnet the upward update, the
downward update and pop the top element of the heap.
A swap function which is used often is also added in the file and it takes
to pointers to swap.
*/



/*
 * find_nodes -- find_nodes recuses itself to find all leaf nodes that can be 
 *  fitted into the array.
 * INPUTS: loc -- locale pointer
 *         vs -- vertex set pointer
 *         p -- pyramid tree pointer
 *         nnum -- starting index
 * OUTPUTS: vs -- vertex set
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */
void
find_nodes (locale_t* loc, vertex_set_t* vs, pyr_tree_t* p, int32_t nnum)
{   
    // Do not modify the following line nor add anything above in the function.
    record_fn_call ();
    // If leaf node reached, stop recursion.
    if(nnum*4+1 > p->n_nodes-1){
        // Only vertex in the circle will be added to the array.
        if(in_range(loc,p->node[nnum].x,p->node[nnum].y_left)){
            vs->id[vs->count]=p->node[nnum].id;
            vs->count++;
        }   
        return;
    }
    // start recursion: if this is not the leaf node and no source vertices in the 
    // to be recursed area, than do not recurse that area 
    if(4*nnum+1<p->n_nodes && !(loc->y -  p->node[nnum].y_left > loc->range     //upper left
        || loc->x - p->node[nnum].x > loc->range))    find_nodes(loc,vs,p,nnum*4+1);
    if(4*nnum+2<p->n_nodes && !(loc->y -  p->node[nnum].y_right > loc->range    //upper right
        || p->node[nnum].x - loc->x > loc->range))    find_nodes(loc,vs,p,nnum*4+2);
    if(4*nnum+3<p->n_nodes && !(p->node[nnum].y_left - loc->y  > loc->range     //lower left
        || loc->x - p->node[nnum].x > loc->range))    find_nodes(loc,vs,p,nnum*4+3);
    if(4*nnum+4<p->n_nodes && !(p->node[nnum].y_right - loc->y > loc->range     //lower right
        || p->node[nnum].x - loc->x > loc->range))    find_nodes(loc,vs,p,nnum*4+4);

}


/*
 * trim_nodes -- find_nodes recuses itself to find all leaf nodes that can be 
 *               fitted into the array.
 * INPUTS: g -- graph pointer
 *         vs -- vertex set pointer
 *         loc -- pyramid tree pointer
 * OUTPUTS: vertex set updated with vertices within range
 * RETURN VALUE: vs -- vertex set
 * SIDE EFFECTS: none
 */
void
trim_nodes (graph_t* g, vertex_set_t* vs, locale_t* loc)
{
    int32_t cnt = 0;    //new veretex set counter
    for (int32_t i=0;i<vs->count;i++){
        if(in_range(loc,g->vertex[vs->id[i]].x,g->vertex[vs->id[i]].y)){
            vs->id[cnt]=vs->id[i];  // set the current array at index cnt
            cnt++;
        } 
    }
    vs->count = cnt;    //update vertex set
}


/*
 * swap -- swap the two values of the pointers.
 * INPUTS: a -- value pointer
 *         v -- value pointer
 * OUTPUTS: *a -- vaule of a
 *          *b -- value of b
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */
void swap(int32_t* a, int32_t* b){
    int32_t temp; //temporary container
    temp = *a;
    *a = *b;
    *b= temp;
}


/*
 * up_update -- update the heap from the leaf nodes to its top by recusing to
 *               let the top always contain the smallest from source distance.
 * INPUTS: g -- graph pointer
 *         h -- heap pointer
 *         index -- heap children node index
 * OUTPUTS: h -- updated heap
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */
void 
up_update(graph_t* g, heap_t* h, int32_t index){
    // creat pointers to parent and children node
    int32_t* parent;
    int32_t* children1;
    int32_t* children2;
    if(index == 0){
       return;
    }
    // initialise pointers
    parent = &(h->elt[(index-1)/2]);
    children1 = &(h->elt[index]);
    if((index+1>=h->n_elts)&&(index%2==1)){    // if index+1 node out of bound and index node is odd
        if(g->vertex[*parent].from_src>g->vertex[*children1].from_src){
            swap(parent,children1);
            swap(&(g->vertex[*parent].heap_id),&(g->vertex[*children1].heap_id));
        }
    }
    else if(index%2==1){    // if index+1 node in bound and index node is odd
        children2 = &(h->elt[index+1]);
        if(g->vertex[*parent].from_src>g->vertex[*children1].from_src){
            swap(parent,children1);     // update heap
            swap(&(g->vertex[*parent].heap_id),&(g->vertex[*children1].heap_id));  // update vertex
        }else if(g->vertex[*parent].from_src>g->vertex[*children2].from_src){
            swap(parent,children2);
            swap(&(g->vertex[*parent].heap_id),&(g->vertex[*children2].heap_id));
        }
    }
    else if(index%2==0){     // if index+1 node in bound and index node is even
        children2 = &(h->elt[index-1]);
        if(g->vertex[*parent].from_src>g->vertex[*children1].from_src){
            swap(parent,children1);
            swap(&g->vertex[*parent].heap_id,&g->vertex[*children1].heap_id);
        }else if(g->vertex[*parent].from_src>g->vertex[*children2].from_src){
            swap(parent,children2);
            swap(&(g->vertex[*parent].heap_id),&(g->vertex[*children2].heap_id));
        }
    }
    up_update(g,h,(index - 1)/2);   //recurse
}


/*
 *  down_update -- update the heap from the top to its leafs by recusing to
 *               let the top always contain the smallest from source distance,
 *               and its leaf from source distance larger than the parent.
 * INPUTS: g -- graph pointer
 *         h -- heap pointer
 *         index -- heap parent index 
 * OUTPUTS: h -- updated heap
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */
void down_update(graph_t* g, heap_t* h, int32_t index){
    // creat pointers to parent and children node
    int32_t* parent = &(h->elt[index]);
    int32_t* children1 = &(h->elt[index * 2 + 1]);
    int32_t* children2 = &(h->elt[index * 2 + 2]);
    // End recursion when reaching the leaf nodes
    if(index*2+1 >= h->n_elts){
        return;
    }
    // children 1 is the smallest the three, swap the parent and the children1 node
    if(g->vertex[*parent].from_src > g->vertex[*children1].from_src
        && g->vertex[*children1].from_src < g->vertex[*children2].from_src){
        swap(parent,children1);
        swap(&(g->vertex[*parent].heap_id),&(g->vertex[*children1].heap_id));
        down_update(g,h,index*2+1); //recurse
    // children 2 is the smallest the three, swap the parent and the children1 node
    // and there exits such children as children2
    }else if(g->vertex[*parent].from_src > g->vertex[*children2].from_src
        && index * 2 + 2 < h->n_elts){
        swap(parent,children2);
        swap(&(g->vertex[*parent].heap_id),&(g->vertex[*children2].heap_id));
        down_update(g,h,index*2+2);
    }
}


/*
 * heap_init -- Initialise the heap. First set all vertices's from source
 *               distance to infinity and pred value to undefined, then 
 *               set the vertices from the source's frm_src value to 0 and
 *               add them to the heap. Finally add all the vertices to the
 *               heap.
 * INPUTS: g -- graph pointer
 *         h -- heap pointer
 *         src -- source vertex array pointer
 * OUTPUTS: h -- heap
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */
void 
heap_init (graph_t* g, heap_t* h, vertex_set_t* src){
    h->n_elts=0;
    // Initialise all the vertices in the graph
    for(int32_t i=0;i < g->n_vertices;i++){
        g->vertex[i].from_src = MY_INFINITY;
        g->vertex[i].pred = UNDEFINED;
    }
    // Initialise all the source vertices in the graph and add them to 
    // the smaller index value of the heap.
    for(int32_t i=0;i < src->count;i++){
        // all source points have from source distance 0
        g->vertex[src->id[i]].from_src = 0; 
        g->vertex[src->id[i]].heap_id = h->n_elts;
        h->elt[h->n_elts] = src->id[i];
        h->n_elts++;
    }
    // Initialise the rest vertices in the graph and add them to 
    // the bigger index value of the heap.
    for(int32_t i=0;i < g->n_vertices; i++){
        if(g->vertex[i].from_src!=0){
            h->elt[h->n_elts] = i;
            g->vertex[i].heap_id = h->n_elts;
            h->n_elts++;
        }
    }
}

/*
 * heap_pop -- This funciton pop the top element of the heap and move the last 
 *             element to the top, then update the heap. Return the poped value.
 * INPUTS: g -- graph pointer
 *         h -- heap pointer
 * OUTPUTS: h -- Pop the top element of the heap.
 * RETURN VALUE: value of the popped element of the heap.
 * SIDE EFFECTS: none
 */
int32_t
heap_pop(graph_t* g, heap_t* h){
    int32_t u;
    u = h->elt[0];  // popped value
    h->elt[0] = h->elt[h->n_elts-1];
    // the heap_id of the last element is now 0
    g->vertex[h->elt[h->n_elts-1]].heap_id = 0; 
    h->n_elts--;
    down_update(g,h,0);
    return u;
}


/*
 * dijkstra -- Implenmentation of the dijkstra algorithm, and write the 
 *             the result to struct path.
 * INPUTS: g -- graph pointer
 *         h -- heap pointer
 *         src -- source vertex set pointer
 *         dest -- destination vertex set pointer
 *         path -- path struct pointer
 * OUTPUTS: h -- updated heap
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */
int32_t
dijkstra (graph_t* g, heap_t* h, vertex_set_t* src, vertex_set_t* dest,
          path_t* path)
{
    int32_t alt;    // distance to the source to be updated
    int32_t min_dist = MY_INFINITY;
    int32_t min_dest_id = UNDEFINED;
    int32_t current_id = UNDEFINED;
    heap_init(g,h,src);
    while(h->n_elts!=0){    // continue if the heap is not empty
        int32_t u = heap_pop(g,h);
        int32_t heap_index;
        for(int32_t i=0 ;i < g->vertex[u].n_neighbors; i++){
            // find the shortest distance
            alt = g->vertex[u].from_src + g->vertex[u].distance[i];
            // update node if this is the shortest
            if(alt < g->vertex[g->vertex[u].neighbor[i]].from_src){
                g->vertex[g->vertex[u].neighbor[i]].from_src = alt;
                g->vertex[g->vertex[u].neighbor[i]].pred = u;
                heap_index = g->vertex[g->vertex[u].neighbor[i]].heap_id;
                up_update(g,h,heap_index);
            }
        }
    }
    // find the minimum distance to srouce set from destination set
    for(int32_t i=0; i < dest->count; i++){
        if(g->vertex[dest->id[i]].from_src < min_dist){
            min_dist = g->vertex[dest->id[i]].from_src;
            min_dest_id = dest->id[i];
        }
    }
    current_id = min_dest_id;   // set the current_id to the maximum of the ending vertices
    path->n_vertices = 0;
    path->tot_dist = g->vertex[min_dest_id].from_src;
    if(g->vertex[min_dest_id].pred != UNDEFINED){   
        // continue only when the previous vertex is defined
        //TODO: the conidtion when the target fall in source is not considered
        while(current_id != UNDEFINED){
            // ensure path length whithin range
            if(path->n_vertices + 1 > MAX_PATH_LENGTH){
                return 0;
            }
            // writing the shortest path to the array
            path->id[path->n_vertices] = current_id;
            path->n_vertices++; 
            current_id = g->vertex[current_id].pred;
        }
        // reverse the array to make it goes forward
        for(int32_t i=0;i<path->n_vertices/2;i++){
            swap(&path->id[i],&path->id[path->n_vertices-1-i]);
        }
        return 1;   //return 1 when such path found.
    }
    return 0; //return 0 on failure
}

