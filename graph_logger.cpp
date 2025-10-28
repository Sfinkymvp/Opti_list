#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include "data.h"
#include "graph_logger.h"


static void generateHeaders(List* list, FILE* graph_out)
{
    assert(list != NULL);
    assert(list->storage != NULL);
    assert(graph_out != NULL);

    fprintf(graph_out, "\theaders[shape=\"Mrecord\",label="
                       "\"<h>head = %d | <t>tail = %d | <f>free_head = %d\"];\n",
                       list->storage[0].next, list->storage[0].prev, list->free_head);
    fprintf(graph_out, "\n");
}


static void generateNodes(List* list, FILE* graph_out)
{
    assert(list != NULL);
    assert(list->storage != NULL);
    assert(graph_out != NULL);

    for (int index = list->storage[0].next; index != 0;
         index = list->storage[index].next) {
        fprintf(graph_out, "\tdata%d[shape=\"Mrecord\",label="
                           "\"index = %d | value = " DATATYPE_SPEC " | {prev = %d | next = %d}\"];\n",
        index, index, list->storage[index].value, list->storage[index].prev,
        list->storage[index].next);
    }
    fprintf(graph_out, "\n");
}


static void generateFreeNodes(List* list, FILE* graph_out)
{
    assert(list != NULL);
    assert(list->storage != NULL);
    assert(graph_out != NULL);

    for (int index = list->free_head; index != END_OF_MEMORY;
         index = list->storage[index].next) {
        fprintf(graph_out, "\tfree_data%d[shape=\"Mrecord\",label="
                           "\"index = %d | value = POISON | {prev = %d | next = %d}\"];\n",
        index, index, list->storage[index].prev, list->storage[index].next);
    }
    fprintf(graph_out, "\n");
}


static void connectHeadersToNodes(List* list, FILE* graph_out)
{
    assert(list != NULL);
    assert(list->storage != NULL);
    assert(graph_out != NULL);

    if (list->storage[0].next != 0 && list->storage[0].prev != 0) {
        fprintf(graph_out, "\theaders:h->data%d;\n", list->storage[0].next);
        fprintf(graph_out, "\theaders:t->data%d;\n", list->storage[0].prev);
        fprintf(graph_out, "\n");
    }
}


static void connectNodes(List* list, FILE* graph_out)
{
    assert(list != NULL);
    assert(list->storage != NULL);
    assert(graph_out != NULL);

    for (int index = list->storage[0].next; list->storage[index].next != 0;
         index = list->storage[index].next)
        fprintf(graph_out, "\tdata%d -> data%d;\n",
                index, list->storage[index].next);
    fprintf(graph_out, "\n");
    
    for (int index = list->storage[0].prev; list->storage[index].prev != 0;
         index = list->storage[index].prev)
        fprintf(graph_out, "\tdata%d -> data%d;\n",
                index, list->storage[index].prev);    
    fprintf(graph_out, "\n");
}


static void connectFreeNodes(List* list, FILE* graph_out)
{
    assert(list != NULL);
    assert(list->storage != NULL);
    assert(graph_out != NULL);

    fprintf(graph_out, "\theaders:f->free_data%d;\n", list->free_head);
    for (int index = list->free_head; list->storage[index].next != END_OF_MEMORY;
         index = list->storage[index].next)
        fprintf(graph_out, "\tfree_data%d->free_data%d;\n",
               index, list->storage[index].next);
    fprintf(graph_out, "\n");
}


void generateGraph(List* list, const char* dot_file)
{
    assert(list != NULL);
    assert(list->storage != NULL);
    assert(dot_file != NULL);
    
    FILE* graph_out = fopen(dot_file, "w");
    assert(graph_out != NULL);

    fprintf(graph_out, "digraph G {\n");
    fprintf(graph_out, "\trankdir=LR;\n");
    fprintf(graph_out, "\tsplines=\"ortho\"\n\n");

    generateHeaders(list, graph_out);
    generateNodes(list, graph_out);
    generateFreeNodes(list, graph_out);

    connectHeadersToNodes(list, graph_out);
    connectNodes(list, graph_out);
    connectFreeNodes(list, graph_out);   
    
    fprintf(graph_out, "}\n");

    assert(fclose(graph_out) == 0); 
}


static void convertDotToPng(const char* dot_file, const char* png_file)
{
    assert(dot_file != NULL);
    assert(png_file != NULL);

    char command[BUFFER_SIZE] = {};
    snprintf(command, BUFFER_SIZE, "dot -Tpng %s -o %s",
             dot_file, png_file);
    
    system(command);
}


static void writeListInfo(List* list, DumpInfo* info, FILE* list_out)
{
    assert(list != NULL);
    assert(info != NULL);
    assert(info->source_file != NULL);
    assert(info->function_name != NULL);
    assert(list_out != NULL);

    fprintf(list_out, "\t<h3>LIST DUMP #%03d</h3>\n", info->file_number);
    fprintf(list_out, "\t<p><strong>Dump {%s:%d} called from %s()</strong></p>\n",
            info->source_file, info->line_number, info->function_name);
    fprintf(list_out, "\t<p><strong>List \"%s\" {%s:%d} created in %s()</strong></p>\n",
            list->debug_info.list_name, list->debug_info.source_file, 
            list->debug_info.line_number, list->debug_info.function_name);
  

    fprintf(list_out, "\tNext: \n"); 
    for (int index = list->storage[0].next, count = 0;
         index != 0 && count < list->capacity;
         index = list->storage[index].next, count++)
        fprintf(list_out, "%d ", list->storage[index].next);
    fprintf(list_out, "\n");

    fprintf(list_out, "\n\tPrev: \n");
    for (int index = list->storage[0].prev, count = 0;
         index != 0 && count < list->capacity;
         index = list->storage[index].prev, count++)
        fprintf(list_out, "%d ", list->storage[index].prev);
    fprintf(list_out, "\n");
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

    static int call_dump = 0;
    FILE* list_out = NULL;
    if (call_dump == 0)
        list_out = fopen(list_file, "w");
    else
        list_out = fopen(list_file, "a");
    assert(list_out != NULL);

    fprintf(list_out, "<!DOCTYPE html>\n");
    fprintf(list_out, "<html>\n");
    fprintf(list_out, "<body>\n");

    writeListInfo(list, info, list_out);
    fprintf(list_out, "<img src=\"\n%s\" height=180px>\n", image);

    fprintf(list_out, "</body>\n");
    fprintf(list_out, "</html>\n");

    assert(fclose(list_out) == 0);

    call_dump++;
}


void listDump(List* list, int file_number, const char* filename,
              const char* function_name, int line_number)
{
    assert(list != NULL);
    assert(list->storage != NULL);
    assert(filename != NULL);
    assert(function_name != NULL);

    DumpInfo info = {filename, function_name, line_number, file_number};
    char graph_dot_file[BUFFER_SIZE] = {};
    char graph_png_file[BUFFER_SIZE] = {};
    char list_file[BUFFER_SIZE] = {};
    snprintf(graph_dot_file, BUFFER_SIZE, "graph_dump_%03d.dot", file_number);
    snprintf(graph_png_file, BUFFER_SIZE, "graph_dump_%03d.png", file_number);
    snprintf(list_file, BUFFER_SIZE, "list_dump.html");

    generateGraph(list, graph_dot_file);
    convertDotToPng(graph_dot_file, graph_png_file); 

    char command[BUFFER_SIZE + 10] = {};
    snprintf(command, BUFFER_SIZE + 10, "rm %s", graph_dot_file);
    system(command);
   
    createHtmlDump(list, list_file, &info, graph_png_file);
}
