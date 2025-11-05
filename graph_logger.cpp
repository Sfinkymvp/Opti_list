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
 
    fprintf(graph_out, "\t{rank=same;head->node_%d[color=black];}\n", list->storage[0].next);
    fprintf(graph_out, "\t{rank=same;tail->node_%d[color=black];}\n", list->storage[0].prev);
    fprintf(graph_out, "\t{rank=same;free_head->node_%d[color=black];}\n", list->free_head);

    int error_index = 1000;

    for (int index = list->storage[0].next, count = 0; !(count >= list->capacity ||
         index < 0 || index >= list->capacity) && list->storage[index].next != 0;
         index = list->storage[index].next, count++) {
        if (index == 0)
            continue;
        if (list->storage[index].prev < 0 || list->storage[index].prev >= list->capacity) {
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
    fprintf(graph_out, "\tsplines=ortho\n");

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


static void writeListInfo(List* list, DumpInfo* info)
{
    assert(list != NULL);
    assert(info != NULL);
    assert(info->file != NULL);
    assert(info->function != NULL);
    assert(list->debug_info.dump.file != NULL);

    fprintf(list->debug_info.dump.file, "\t<h1>LIST DUMP #%03d</h1>\n", list->debug_info.dump.image_counter);
    fprintf(list->debug_info.dump.file, "\t<h2>Dump {%s:%d} called from %s()</h2>\n",
            info->file, info->line, info->function);
    fprintf(list->debug_info.dump.file, "\t<h2>List \"%s\" {%s:%d} created in %s()</h2>\n",
            list->debug_info.creation.list_name, list->debug_info.creation.file, 
            list->debug_info.creation.line, list->debug_info.creation.function);
}


static void writeListData(List* list)
{
    assert(list != NULL);
    assert(list->storage != NULL);
    assert(list->debug_info.dump.file != NULL);

    fprintf(list->debug_info.dump.file, "<div style=\"white-space: pre;"
            "overflow-x: auto; font-family: monospace;\">");

    fprintf(list->debug_info.dump.file, "\t<div><strong>Size:</strong> %d</div>",
            list->size);
    fprintf(list->debug_info.dump.file, "\t<div><strong>Capacity:</strong> %d</div>",
            list->capacity);
    fprintf(list->debug_info.dump.file, "\t<br><strong>Node:</strong>  |");
    for (int index = 0; index < list->capacity; index++)
        fprintf(list->debug_info.dump.file, " %11d |", index);

    fprintf(list->debug_info.dump.file, "\n\t<br><strong>next:</strong>  |");
    for (int index = 0; index < list->capacity; index++)
        fprintf(list->debug_info.dump.file, " %11d |", list->storage[index].next);

    fprintf(list->debug_info.dump.file, "\n\t<br><strong>value:</strong> |");
    for (int index = 0; index < list->capacity; index++) {
        if (index == 0)
            fprintf(list->debug_info.dump.file, "     FICTIVE |");
        else if (list->storage[index].value == LIST_POISON_VALUE)
            fprintf(list->debug_info.dump.file, "      POISON |");
        else
            fprintf(list->debug_info.dump.file, " %11d |", list->storage[index].value);
    }

    fprintf(list->debug_info.dump.file, "\n\t<br><strong>prev:</strong>  |"); 
    for (int index = 0; index < list->capacity; index++)
        fprintf(list->debug_info.dump.file, " %11d |", list->storage[index].prev);
    fprintf(list->debug_info.dump.file, "\n<br>");

    fprintf(list->debug_info.dump.file, "</div>\n");
}


static void createHtmlDump(List* list, DumpInfo* info, const char* image)
{
    assert(list != NULL);
    assert(info != NULL);
    assert(info->file != NULL);
    assert(info->function != NULL);
    assert(list->debug_info.dump.file != NULL);
    assert(image != NULL);

    fprintf(list->debug_info.dump.file, "<!DOCTYPE html>\n");
    fprintf(list->debug_info.dump.file, "<html>\n");
    fprintf(list->debug_info.dump.file, "<style>\n");
    fprintf(list->debug_info.dump.file, "body { font-family: monospace; }\n");
    fprintf(list->debug_info.dump.file, "</style>\n");
    fprintf(list->debug_info.dump.file, "<body>\n");

    writeListInfo(list, info);
    writeListData(list);

    fprintf(list->debug_info.dump.file, "<div style=\"overflow-x: auto; white-space: nowrap;\">\n");
    fprintf(list->debug_info.dump.file, "<img src=\"list_graph_%03d.svg\" "
            "style=\"zoom:0.65; -moz-transform:scale(0.1); -moz-transform-origin:top left;\">\n",
            list->debug_info.dump.image_counter);
    fprintf(list->debug_info.dump.file, "</div>\n");

    fprintf(list->debug_info.dump.file, "<hr style=\"margin: 40px 0; border: 2px solid #ccc;\">\n");
    fprintf(list->debug_info.dump.file, "</body>\n");
    fprintf(list->debug_info.dump.file, "</html>\n");
}


void listDump(List* list, const char* file, const char* function, int line)
{
    assert(list != NULL);
    assert(list->storage != NULL);
    assert(list->debug_info.dump.file != NULL);
    assert(file != NULL);
    assert(function != NULL);

    DumpInfo info = {file, function, line};
    char graph_dot_file[BUFFER_SIZE * 2] = {};
    char graph_svg_file[BUFFER_SIZE * 2] = {};
   
    snprintf(graph_dot_file, BUFFER_SIZE * 2, "%s/list_graph_%03d.dot",
             list->debug_info.dump.directory, list->debug_info.dump.image_counter);
    snprintf(graph_svg_file, BUFFER_SIZE * 2, "%s/list_graph_%03d.svg",
             list->debug_info.dump.directory, list->debug_info.dump.image_counter);

    generateGraph(list, graph_dot_file);
    convertDotToSvg(graph_dot_file, graph_svg_file); 

    char command[BUFFER_SIZE * 3] = {};
    snprintf(command, BUFFER_SIZE * 3, "rm %s", graph_dot_file);
    system(command);
   
    createHtmlDump(list, &info, graph_svg_file);

    list->debug_info.dump.image_counter++;
}
