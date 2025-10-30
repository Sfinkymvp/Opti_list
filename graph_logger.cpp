#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include "data.h"
#include "graph_logger.h"


const char* IMAGE_DIRECTORY = "images";


static void generateNodes(List* list, FILE* graph_out)
{
    assert(list != NULL);
    assert(list->storage != NULL);
    assert(graph_out != NULL);

    fprintf(graph_out, "\thead[shape=\"Mrecord\",label=\"HEAD\",fontname=\"Monospace\""
                       "fillcolor=\"#E0E0E0\",color=\"#424242\",style=filled,penwidth=2.0];\n");
    fprintf(graph_out, "\ttail[shape=\"Mrecord\",label=\"TAIL\",fontname=\"Monospace\""
                       "fillcolor=\"#E0E0E0\",color=\"#424242\",style=filled,penwidth=2.0];\n");
    fprintf(graph_out, "\tfree_head[shape=\"Mrecord\",fontname=\"Monospace\",label=\"FREE HEAD\","
                       "fillcolor=\"#E0E0E0\",color=\"#424242\",style=filled,penwidth=2.0];\n");

    fprintf(graph_out, "\tnode_0[shape=\"Mrecord\",style=filled,fontname=\"Monospace\","
                       "fillcolor=\"#FFF9C4\",color=\"#E3DC95\",penwidth=2.0,label="
                       "\"head = %d | tail = %d | free_head = %d\"];\n",
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
    assert(list != NULL);
    assert(list->storage != NULL);
    assert(graph_out != NULL);

    fprintf(graph_out, "edge[style=\"invis\"]\n");
    for (int index = 0; index < list->capacity - 1; index++)
        fprintf(graph_out, "\tnode_%d->node_%d;\n", index, index + 1);
    fprintf(graph_out, "\tedge[style=\"solid\",constraint=false]\n");
 
    fprintf(graph_out, "\t{rank=same;head->node_%d[color=\"#01579B\"];}\n", list->storage[0].next);
    fprintf(graph_out, "\t{rank=same;tail->node_%d[color=red4];}\n", list->storage[0].prev);
    fprintf(graph_out, "\t{rank=same;free_head->node_%d[color=\"#2E7D32\"];}\n", list->free_head);

    int error_index = 1000;
    for (int index = list->storage[0].next, count = 0; !(count >= list->capacity ||
         index < 0 || index >= list->capacity) && list->storage[index].next != 0;
         index = list->storage[index].next, count++) {
        if (list->storage[index].next < 0 || list->storage[index].next >= list->capacity) {
            fprintf(graph_out, "\tnode_%d[shape=\"octagon\",fontname=\"Monospace\",fillcolor=brown1,"
                               "color=red4,penwidth=2.0,style=filled,label=\"%d\"];\n", 
                               error_index, list->storage[index].next);
            fprintf(graph_out, "\tnode_%d->node_%d[color=\"#01579B\"];\n",
                               index, error_index);
            error_index++;
        }
        else
            fprintf(graph_out, "\tnode_%d->node_%d[color=\"#01579B\"];\n",
                index, list->storage[index].next);
    }
    fprintf(graph_out, "\n");
    
    for (int index = list->storage[0].next, count = 0; 
         !(count >= list->capacity || index < 0 || index >= list->capacity) && index != 0;
         index = list->storage[index].next, count++) {
        if (list->storage[index].prev == 0)
            continue;
        if (list->storage[index].prev < 0 || list->storage[index].prev >= list->capacity) {
            fprintf(graph_out, "\tnode_%d[shape=\"octagon\",fontname=\"Monospace\",fillcolor=brown1,"
                               "color=red4,penwidth=2.0,style=filled,label=\"%d\"];\n", 
                               error_index, list->storage[index].prev);
            fprintf(graph_out, "\tnode_%d->node_%d[color=red4];\n",
                               index, error_index);
            error_index++;
        } else
            fprintf(graph_out, "\tnode_%d->node_%d[color=red4];\n",
                               index, list->storage[index].prev);
    }
    fprintf(graph_out, "\n");

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

static void generateGraph(List* list, const char* dot_file)
{
    assert(list != NULL);
    assert(list->storage != NULL);
    assert(dot_file != NULL);
    
    FILE* graph_out = fopen(dot_file, "w");
    assert(graph_out != NULL);

    fprintf(graph_out, "digraph G {\n");
    fprintf(graph_out, "\trankdir=LR\n");
    fprintf(graph_out, "\tsplines=\"ortho\"\n");

    generateNodes(list, graph_out);

    connectNodes(list, graph_out);
   
    fprintf(graph_out, "}\n"); 

    assert(fclose(graph_out) == 0); 
}


static void convertDotToSvg(const char* dot_file, const char* svg_file)
{
    assert(dot_file != NULL);
    assert(svg_file != NULL);

    char command[BUFFER_SIZE] = {};
    snprintf(command, BUFFER_SIZE, "dot -Tsvg %s -o %s",
             dot_file, svg_file);
    
    system(command);
}


static void writeListInfo(List* list, DumpInfo* info, FILE* list_out)
{
    assert(list != NULL);
    assert(info != NULL);
    assert(info->source_file != NULL);
    assert(info->function_name != NULL);
    assert(list_out != NULL);

    fprintf(list_out, "\t<h2>LIST DUMP #%03d</h2>\n", info->dump_counter);
    fprintf(list_out, "\t<h3>Dump {%s:%d} called from %s()</h3>\n",
            info->source_file, info->line_number, info->function_name);
    fprintf(list_out, "\t<h3>List \"%s\" {%s:%d} created in %s()</h3>\n",
            list->debug_info.list_name, list->debug_info.source_file, 
            list->debug_info.line_number, list->debug_info.function_name);
}


static void writeListData(List* list, FILE* list_out)
{
    assert(list != NULL);
    assert(list->storage != NULL);
    assert(list_out != NULL);
    fprintf(list_out, "<div style=\"white-space: pre-wrap; font-family: monospace;\">");

    fprintf(list_out, "\t<br><strong>Node:</strong>  |");
    for (int index = 0; index < list->capacity; index++)
        fprintf(list_out, " %11d |", index);

    fprintf(list_out, "\n\t<br><strong>next:</strong>  |");
    for (int index = 0; index < list->capacity; index++)
        fprintf(list_out, " %11d |", list->storage[index].next);

    fprintf(list_out, "\n\t<br><strong>value:</strong> |");
    for (int index = 0; index < list->capacity; index++) {
        if (index == 0)
            fprintf(list_out, "     FICTIVE |");
        else if (list->storage[index].value == LIST_POISON_VALUE)
            fprintf(list_out, "      POISON |");
        else
            fprintf(list_out, " %11d |", list->storage[index].value);
    }

    fprintf(list_out, "\n\t<br><strong>prev:</strong>  |"); 
    for (int index = 0; index < list->capacity; index++)
        fprintf(list_out, " %11d |", list->storage[index].prev);
    fprintf(list_out, "\n<br>");

    fprintf(list_out, "</div>\n");
}


static void createHtmlDump(List* list, const char* list_file,
                           DumpInfo* info, const char* image)
{
    assert(list != NULL);
    assert(list_file != NULL);
    assert(info != NULL);
    assert(info->source_file != NULL);
    assert(info->function_name != NULL);
    assert(image != NULL);

    FILE* list_out = NULL;
    if (info->dump_counter == 1)
        list_out = fopen(list_file, "w");
    else
        list_out = fopen(list_file, "a");
    assert(list_out != NULL);

    fprintf(list_out, "<!DOCTYPE html>\n");
    fprintf(list_out, "<html>\n");
    fprintf(list_out, "<style>\n");
    fprintf(list_out, "body { font-family: monospace; }\n");
    fprintf(list_out, "</style>\n");
    fprintf(list_out, "<body>\n");

    writeListInfo(list, info, list_out);
    writeListData(list, list_out);
    fprintf(list_out, "<img src=\"\n%s\" height=140px>\n", image);

    fprintf(list_out, "<hr style=\"margin: 40px 0; border: 2px solid #ccc;\">\n");
    fprintf(list_out, "</body>\n");
    fprintf(list_out, "</html>\n");

    assert(fclose(list_out) == 0);
}


void listDump(List* list, const char* filename,
              const char* function_name, int line_number)
{
    assert(list != NULL);
    assert(list->storage != NULL);
    assert(filename != NULL);
    assert(function_name != NULL);

    static int dump_counter = 1;

    DumpInfo info = {filename, function_name, line_number, dump_counter};
    char graph_dot_file[BUFFER_SIZE] = {};
    char graph_svg_file[BUFFER_SIZE] = {};
    char list_file[BUFFER_SIZE] = {};
    snprintf(graph_dot_file, BUFFER_SIZE, "%s/graph_dump_%03d.dot", IMAGE_DIRECTORY, dump_counter);
    snprintf(graph_svg_file, BUFFER_SIZE, "%s/graph_dump_%03d.svg", IMAGE_DIRECTORY, dump_counter);
    snprintf(list_file, BUFFER_SIZE, "list_dump.html");

    generateGraph(list, graph_dot_file);
    convertDotToSvg(graph_dot_file, graph_svg_file); 

    char command[BUFFER_SIZE + 10] = {};
    snprintf(command, BUFFER_SIZE + 10, "rm %s", graph_dot_file);
    //system(command);
   
    createHtmlDump(list, list_file, &info, graph_svg_file);

    dump_counter++;
}
