#include <stdio.h>
#include <assert.h>


#include "graph_generator.h"
#include "data.h"


static void generateNodes(List* list, FILE* graph_out)
{
    assert(list); assert(list->storage); assert(graph_out);

    fprintf(graph_out, "\thead[shape=\"Mrecord\",label=\"HEAD\",fontname=\"Monospace\""
                       "fillcolor=\"#E0E0E0\",color=\"#424242\",style=filled,penwidth=2.0];\n");
    fprintf(graph_out, "\ttail[shape=\"Mrecord\",label=\"TAIL\",fontname=\"Monospace\""
                       "fillcolor=\"#E0E0E0\",color=\"#424242\",style=filled,penwidth=2.0];\n");
    fprintf(graph_out, "\tfree_head[shape=\"Mrecord\",fontname=\"Monospace\",label=\"FREE HEAD\","
                       "fillcolor=\"#E0E0E0\",color=\"#424242\",style=filled,penwidth=2.0];\n");

    fprintf(graph_out, "\tnode_0[shape=\"Mrecord\",style=filled,fontname=\"Monospace\","
                       "fillcolor=\"#FFF9C4\",color=\"#E3DC95\",penwidth=2.0,label="
                       "\"head = %d | tail = %d | free head = %d\"];\n",
                       list->storage[0].next, list->storage[0].prev, list->free_head);

    for (int index = 1; index < list->capacity; index++) {
        if (list->storage[index].prev == EMPTY && list->storage[index].value == LIST_POISON_VALUE)
            fprintf(graph_out, "\tnode_%d[shape=\"Mrecord\",fontname=\"Monospace\",fillcolor=\"#C8E6C9\","
                               "color=\"#2E7D32\",penwidth=2.0,style=filled,label="
                               "\"index = %d | value = POISON | "
                               "{prev = %d | next = %d}\"];\n",
                               index, index, list->storage[index].prev, list->storage[index].next);
        else if (list->storage[index].prev == EMPTY ^ list->storage[index].value == LIST_POISON_VALUE)
            fprintf(graph_out, "\tnode_%d[shape=\"Mrecord\",fontname=\"Monospace\",fillcolor=brown1,"
                               "color=red4,penwidth=2.0,style=filled,label="
                               "\"index = %d | value = " DATATYPE_SPEC " | "
                               "{prev = %d | next = %d}\"];\n",
                               index, index, list->storage[index].value,
                               list->storage[index].prev, list->storage[index].next);
        else
            fprintf(graph_out, "\tnode_%d[shape=\"Mrecord\",fontname=\"Monospace\",fillcolor=\"#B3E5FC\","
                               "color=\"#01579B\",penwidth=2.0,style=filled,label="
                               "\"index = %d | value = " DATATYPE_SPEC " | "
                               "{prev = %d | next = %d}\"];\n",
                               index, index, list->storage[index].value,
                               list->storage[index].prev, list->storage[index].next);
    }
    fprintf(graph_out, "\n");
}


static void connectNodes(List* list, FILE* graph_out)
{
    assert(list); assert(list->storage); assert(graph_out);

    fprintf(graph_out, "\tedge[style=\"invis\"]\n");
    for (int index = 0; index < list->capacity - 1; index++)
        fprintf(graph_out, "\tnode_%d->node_%d;\n", index, index + 1);
    fprintf(graph_out, "\tedge[style=\"solid\",constraint=false]\n");

    fprintf(graph_out, "\t{rank=same;head->node_%d[color=black];}\n", list->storage[0].next);
    fprintf(graph_out, "\t{rank=same;tail->node_%d[color=black];}\n", list->storage[0].prev);
    if (list->free_head != EMPTY)
        fprintf(graph_out, "\t{rank=same;free_head->node_%d[color=black];}\n", list->free_head);

    int error_index = 1000;
    for (int index = 0, count = 0; count < list->capacity &&
         index >= 0 && index < list->capacity; index = list->storage[index].next, count++) {
        if (list->storage[index].next >= 0 && list->storage[index].next < list->capacity) {
            if (list->storage[list->storage[index].next].prev != index) {
                if (list->storage[list->storage[index].next].prev < 0 ||
                    list->storage[list->storage[index].next].prev >= list->capacity) {
                    fprintf(graph_out, "\tnode_%d[shape=\"octagon\",fontname=\"Monospace\",fillcolor=brown1,"
                                       "color=red4,penwidth=2.0,style=filled,label=\"%d\"];\n", 
                                       error_index, list->storage[list->storage[index].next].prev);
                    fprintf(graph_out, "\tnode_%d->node_%d[color=brown1, penwidth=3];\n",
                                       error_index, list->storage[index].next);
                    error_index++;
                } else {
                    fprintf(graph_out, "\tnode_%d->node_%d[color=brown1, penwidth=3];\n",
                                       list->storage[list->storage[index].next].prev,
                                       list->storage[index].next);
                }

                fprintf(graph_out, "\tnode_%d->node_%d[color=brown1, penwidth=3];\n",
                        index, list->storage[index].next);
            }
        } else
            break;
    }

    for (int index = 0, count = 0; count < list->capacity &&
         index >= 0 && index < list->capacity; index = list->storage[index].prev, count++) {
        if (list->storage[index].prev >= 0 && list->storage[index].prev < list->capacity) {
            if (list->storage[list->storage[index].prev].next != index) {
                if (list->storage[list->storage[index].prev].next < 0 ||
                    list->storage[list->storage[index].prev].next >= list->capacity) {
                    fprintf(graph_out, "\tnode_%d[shape=\"octagon\",fontname=\"Monospace\",fillcolor=brown1,"
                                       "color=red4,penwidth=2.0,style=filled,label=\"%d\"];\n", 
                                       error_index, list->storage[list->storage[index].prev].next);
                    fprintf(graph_out, "\tnode_%d->node_%d[color=brown2, penwidth=3];\n",
                                       list->storage[index].prev, error_index);
                    error_index++;
                } else {
                    fprintf(graph_out, "\tnode_%d->node_%d[color=brown1, penwidth=3];\n",
                                       list->storage[index].prev,
                                       list->storage[list->storage[index].prev].next);
                }

                fprintf(graph_out, "\tnode_%d->node_%d[color=brown1, penwidth=3];\n",
                        list->storage[index].prev, index);
            }
        }
    }
    fprintf(graph_out, "\n");

    for (int index = list->storage[0].next, count = 0; count < list->capacity &&
        index >= 0 && index < list->capacity; index = list->storage[index].next, count++) {
        if (list->storage[index].prev >= 0 && list->storage[index].prev < list->capacity &&
            list->storage[list->storage[index].prev].next == index)
            fprintf(graph_out, "\tnode_%d->node_%d[dir=both, constraint=false, color=black];\n",
                    list->storage[index].prev, index);
        if (index == 0 || (list->storage[index].next == 0 && list->storage[index].prev == 0))
            break;
    }
    
    if (list->free_head == EMPTY)
        return;
    for (int index = list->free_head, count = 0; !(count >= list->capacity ||
         (index < 0 && index != EMPTY) || index >= list->capacity) &&
         list->storage[index].next != EMPTY;  index = list->storage[index].next, count++) {
        if (list->storage[index].next < 0 || list->storage[index].next >= list->capacity) {
            fprintf(graph_out, "\tnode_%d[shape=\"octagon\",fontname=\"Monospace\",fillcolor=brown1,"
                               "color=red4,penwidth=2.0,style=filled,label=\"%d\"];\n", 
                               error_index, list->storage[index].prev);
            fprintf(graph_out, "\tnode_%d->node_%d[color=\"#2E7D32\"];\n",
                               index, error_index);
            error_index++;
        } else
            fprintf(graph_out, "\tnode_%d->node_%d[color=\"#2E7D32\"];\n",
                index, list->storage[index].next);
    }
    fprintf(graph_out, "\n");
} 

void generateGraph(List* list, const char* dot_file)
{
    assert(list); assert(list->storage); assert(dot_file);
    
    FILE* graph_out = fopen(dot_file, "w");
    assert(graph_out != NULL);

    fprintf(graph_out, "digraph G {\n");
    fprintf(graph_out, "\trankdir=LR\n");
    fprintf(graph_out, "\tgraph[splines=ortho]\n");
    fprintf(graph_out, "\tnodesep=0.7;\n");

    generateNodes(list, graph_out);

    connectNodes(list, graph_out);
   
    fprintf(graph_out, "}\n"); 

    assert(fclose(graph_out) == 0); 
}
