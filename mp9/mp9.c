#include <stdint.h>
#include <stdio.h>

#include "mp5.h"
#include "mp9.h"

#define MY_INFINITY   1000000000
#define UNDEFINED    -1


void
find_nodes (locale_t* loc, vertex_set_t* vs, pyr_tree_t* p, int32_t nnum)
{
    // Do not modify the following line nor add anything above in the function.
    record_fn_call ();
    
    if(nnum*4+1 > p->n_nodes-1){
        if(in_range(loc,p->node[nnum].x,p->node[nnum].y_left)){
            vs->id[vs->count]=p->node[nnum].id;
            vs->count++;
        }   
        return;
    }
    if(/*loc->x <= p->node[nnum].x && loc->y <= p->node[nnum].y_left  &&*/ 4*nnum+1<p->n_nodes)    find_nodes(loc,vs,p,nnum*4+1);
    if(/*loc->x >= p->node[nnum].x && loc->y <= p->node[nnum].y_right && */4*nnum+2<p->n_nodes)    find_nodes(loc,vs,p,nnum*4+2);
    if(/*loc->x >= p->node[nnum].x && loc->y >= p->node[nnum].y_left  && */4*nnum+3<p->n_nodes)    find_nodes(loc,vs,p,nnum*4+3);
    if(/*loc->x >= p->node[nnum].x && loc->y >= p->node[nnum].y_right && */4*nnum+4<p->n_nodes)    find_nodes(loc,vs,p,nnum*4+4);
}

void
trim_nodes (graph_t* g, vertex_set_t* vs, locale_t* loc)
{
    for(int32_t i=0;i < vs->count;i++){
        if(!in_range(loc,g->vertex[vs->id[i]].x,g->vertex[vs->id[i]].y)){
            vs->id[i]=vs->id[vs->count-1];    // if current vertex our of range, put the final value to this place
            vs->count--;    // vertex number decrement by 1
            i--;            // next loop continue this place
        }
    }
}

// swap the two values
void swap(int32_t* a, int32_t* b){
    int32_t temp;
    temp = *a;
    *a = *b;
    *b= temp;
}

/*
This function update the heap from the top to its leafs.
*/
void 
up_update(graph_t* g, heap_t* h, int32_t index){
    int32_t* parent;
    int32_t* children1;
    int32_t* children2;
    if(index == 0){
       return;
    }
    parent = &(h->elt[(index-1)/2]);
    children1 = &(h->elt[index]);
    if((index+1>=h->n_elts)&&(index%2==1)){    // if index+1 node out of bound and index node is odd
        if(g->vertex[*parent].from_src>g->vertex[*children1].from_src){
            swap(parent,children1);
            // printf("(%d,%d) heapid:%d ",g->vertex[*parent].x,g->vertex[*parent].y,g->vertex[*parent].heap_id);
            swap(&(g->vertex[*parent].heap_id),&(g->vertex[*children1].heap_id));
            // printf("(%d,%d) heapid:%d\n ",g->vertex[*parent].x,g->vertex[*parent].y, g->vertex[*parent].heap_id);
        }
    }
    else if(index%2==1){    // if index+1 node in bound and index node is even
        children2 = &(h->elt[index+1]);
        if(g->vertex[*parent].from_src>g->vertex[*children1].from_src){
            swap(parent,children1);
            swap(&(g->vertex[*parent].heap_id),&(g->vertex[*children1].heap_id));
        }else if(g->vertex[*parent].from_src>g->vertex[*children2].from_src){
            swap(parent,children2);
            swap(&(g->vertex[*parent].heap_id),&(g->vertex[*children2].heap_id));
        }
    }
    else if(index%2==0){
        children2 = &(h->elt[index-1]);
        if(g->vertex[*parent].from_src>g->vertex[*children1].from_src){
            swap(parent,children1);
            swap(&g->vertex[*parent].heap_id,&g->vertex[*children1].heap_id);
        }else if(g->vertex[*parent].from_src>g->vertex[*children2].from_src){
            swap(parent,children2);
            swap(&(g->vertex[*parent].heap_id),&(g->vertex[*children2].heap_id));
        }
    }
    up_update(g,h,(index - 1)/2);
}

void down_update(graph_t* g, heap_t* h, int32_t index){
    //printf("number of heap: %d\n",h->n_elts);
    //printf("index 1:%d\t",index);
    
    int32_t* parent = &(h->elt[index]);
    int32_t* children1 = &(h->elt[index * 2 + 1]);
    int32_t* children2 = &(h->elt[index * 2 + 2]);
    if(index*2+1 >= h->n_elts){
        return;
    }
    //printf("index:%d,%d,%d,%d\t",index,g->vertex[*parent].from_src,g->vertex[*children1].from_src,g->vertex[*children2].from_src); 
    if(g->vertex[*parent].from_src > g->vertex[*children1].from_src
        && g->vertex[*children1].from_src < g->vertex[*children2].from_src){
        swap(parent,children1);
        swap(&(g->vertex[*parent].heap_id),&(g->vertex[*children1].heap_id));
        down_update(g,h,index*2+1);
        
    }else if(g->vertex[*parent].from_src > g->vertex[*children2].from_src
        && index * 2 + 2 < h->n_elts){
        swap(parent,children2);
        swap(&(g->vertex[*parent].heap_id),&(g->vertex[*children2].heap_id));
        down_update(g,h,index*2+2);
    }
}

void show_heap(graph_t* g,heap_t* h, int32_t elements){
    int32_t i = 1;
    while(i<elements){
        for(int32_t j = 0; j<i; j++){
            printf("%d ",g->vertex[h->elt[i+j-1]].from_src);
        }
        printf("\n");
        i *= 2;
    }
}

// void show_graph(graph_t* g){
//     for(int32_t i=0;i<g->n_vertices;i++){
//         printf("(id:%d):dis %d;pre %d\t",i,g->vertex[i].from_src,g->vertex[i].pred);
//     }
// }

void 
heap_init (graph_t* g, heap_t* h, vertex_set_t* src){
    h->n_elts=0;
    for(int32_t i=0;i < g->n_vertices;i++){
        g->vertex[i].from_src = MY_INFINITY;
        g->vertex[i].pred = UNDEFINED;
    }
    for(int32_t i=0;i < src->count;i++){
        g->vertex[src->id[i]].from_src = 0;
        g->vertex[src->id[i]].heap_id = h->n_elts;
        h->elt[h->n_elts] = src->id[i];
        h->n_elts++;
    }
    for(int32_t i=0;i < g->n_vertices; i++){
        if(g->vertex[i].from_src!=0){
            h->elt[h->n_elts] = i;
            g->vertex[i].heap_id = h->n_elts;
            h->n_elts++;
        }
    }
}

int32_t
heap_pop(graph_t* g, heap_t* h){
    int32_t u;
    u = h->elt[0];
    h->elt[0] = h->elt[h->n_elts-1];
    h->n_elts--;
    down_update(g,h,0);
    //printf("%d \t",g->vertex[h->elt[0]].from_src);
    return u;
}

int32_t
dijkstra (graph_t* g, heap_t* h, vertex_set_t* src, vertex_set_t* dest,
          path_t* path)
{
    int32_t alt;
    int32_t min_dist = MY_INFINITY;
    int32_t min_dest_id = UNDEFINED;
    int32_t current_id = UNDEFINED;
    heap_init(g,h,src);
    //show_graph(g);
    while(h->n_elts!=0){
        int32_t u = heap_pop(g,h);
        //static int32_t j = 1; if(j==2||j==3) show_heap(h,100);j++;
        //printf("u: %d ",u);
        //static int32_t j = 1; if(j==2) show_graph(g);j++;
        int32_t heap_index;
        for(int32_t i=0 ;i < g->vertex[u].n_neighbors; i++){
            alt = g->vertex[u].from_src + g->vertex[u].distance[i];
            //printf("%d ",alt>MY_INFINITY);
            if(alt < g->vertex[g->vertex[u].neighbor[i]].from_src){
                g->vertex[g->vertex[u].neighbor[i]].from_src = alt;
                g->vertex[g->vertex[u].neighbor[i]].pred = u;
                heap_index = g->vertex[g->vertex[u].neighbor[i]].heap_id;
                //printf("heap_index: %d\n",heap_index);
                up_update(g,h,heap_index);
            }
        }
    }
    // find the minimum distance to srouce set from destination set
    //printf("%d\n",min_dest_id);
    //show_graph(g);
    for(int32_t i=0; i < dest->count; i++){
        // printf("g->vertex[dest->id[i]].from_src < min_dist: %d\n",g->vertex[dest->id[i]].from_src < min_dist);
        if(g->vertex[dest->id[i]].from_src < min_dist){
            min_dist = g->vertex[dest->id[i]].from_src;
            min_dest_id = dest->id[i];
        }
    }
    current_id = min_dest_id;
    //printf("%d\n",min_dest_id);
    path->n_vertices = 0;
    path->tot_dist = g->vertex[min_dest_id].from_src;
    //printf("current id: %d\n",current_id);
    if(g->vertex[min_dest_id].pred != UNDEFINED){   // the conidtion when the target fall in source is not considered
        while(current_id != UNDEFINED){
            path->id[path->n_vertices] = current_id;
            path->n_vertices++;
            current_id = g->vertex[current_id].pred;
        }
        return 1;
    }
    return 0;
}


//备注：测试不过的可能问题
//最后的坐标点顺序不一致的问题应该是点放进去的顺序不一致
//


