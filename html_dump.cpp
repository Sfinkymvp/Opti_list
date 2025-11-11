#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>


#include "html_dump.h"
#include "data.h"
#include "graph_generator.h"


const char* IMAGE_DIRECTORY = "images";


const char* error_messages[] = {
    "[LIST_OK] List structure is valid",
    "[LIST_CAPACITY_INVALID] Invalid list capacity",
    "[LIST_FREE_HEAD_INVALID] free_head index is invalid",
    "[LIST_ZERO_NODE_CORRUPTED] Zero node corrupted",
    "[LIST_LOOP_DETECTED] Loop detected in list traversal",
    "[LIST_INDEX_OUT_OF_BOUNDS] Node index is out of valid range",
    "[LIST_FREE_NODE_MIXED] Free node not in free list",
    "[LIST_ERR_PREV_NEXT] Invalid prev/next links between nodes",
    "[LIST_SIZE_INVALID] Element count does not match list->size",
    "[LIST_FREE_STRUCTURE_BROKEN] Free list structure corrupted",
    "[LIST_VALUE_INVALID] Invalid value in list",
    "[LIST_OUT_OF_MEMORY] Failed to allocate memory",
    "[LIST_ERR_DUMP_FILE_OPEN] Failed to open dump file for writing"
};

    
static void convertDotToSvg(const char* dot_file, const char* svg_file)
{
    assert(dot_file); assert(svg_file);

    char command[BUFFER_SIZE] = {};
    snprintf(command, BUFFER_SIZE, "dot -Tsvg %s -o %s",
             dot_file, svg_file);
    
    system(command);
}


static void writeListInfo(List* list, DumpInfo* info)
{
    assert(list); assert(info); assert(info->message); assert(info->file);
    assert(info->function); assert(list->debug.dump.file);

    fprintf(list->debug.dump.file, "\t<h1>LIST DUMP #%03d</h1>\n", list->debug.dump.image_counter);
    fprintf(list->debug.dump.file, "\t<h2>Dump {%s:%d} called from function %s()</h2>\n",
            info->file, info->line, info->function);
    fprintf(list->debug.dump.file, "\t<h2>List \"%s\" {%s:%d} created in function %s()</h2>\n",
            list->debug.creation.list_name, list->debug.creation.file, 
            list->debug.creation.line, list->debug.creation.function);
    fprintf(list->debug.dump.file, "\t<h3>ERROR:   %s</h3>\n", error_messages[info->status]);
    fprintf(list->debug.dump.file, "\t<h3>MESSAGE: %s</h3>\n", info->message);
}


static void writeListData(List* list)
{
    assert(list); assert(list->storage); assert(list->debug.dump.file);

    fprintf(list->debug.dump.file, "<div style=\"white-space: pre;"
            "overflow-x: auto; font-family: monospace;\">");

    fprintf(list->debug.dump.file, "\t<div><strong>Size:</strong> %d</div>",
            list->size);
    fprintf(list->debug.dump.file, "\t<div><strong>Capacity:</strong> %d</div>",
            list->capacity);
    fprintf(list->debug.dump.file, "\t<br><strong>Node:</strong>  |");
    for (int index = 0; index < list->capacity; index++)
        fprintf(list->debug.dump.file, " %11d |", index);

    fprintf(list->debug.dump.file, "\n\t<br><strong>next:</strong>  |");
    for (int index = 0; index < list->capacity; index++)
        fprintf(list->debug.dump.file, " %11d |", list->storage[index].next);

    fprintf(list->debug.dump.file, "\n\t<br><strong>value:</strong> |");
    for (int index = 0; index < list->capacity; index++) {
        if (index == 0)
            fprintf(list->debug.dump.file, "     FICTIVE |");
        else if (list->storage[index].value == LIST_POISON_VALUE)
            fprintf(list->debug.dump.file, "      POISON |");
        else
            fprintf(list->debug.dump.file, " %11d |", list->storage[index].value);
    }

    fprintf(list->debug.dump.file, "\n\t<br><strong>prev:</strong>  |"); 
    for (int index = 0; index < list->capacity; index++) {
        if (list->storage[index].prev == EMPTY)
            fprintf(list->debug.dump.file, "       EMPTY |");
        else
            fprintf(list->debug.dump.file, " %11d |", list->storage[index].prev);
    }
    fprintf(list->debug.dump.file, "\n<br>");
    fprintf(list->debug.dump.file, "</div>\n");
}


static void createHtmlDump(List* list, DumpInfo* info, const char* image)
{
    assert(list); assert(info); assert(info->message); assert(info->file); 
    assert(info->function); assert(list->debug.dump.file); assert(image);

    fprintf(list->debug.dump.file, "<html>\n");
    fprintf(list->debug.dump.file, "<style>\n");
    fprintf(list->debug.dump.file, "body { font-family: monospace; }\n");
    fprintf(list->debug.dump.file, "</style>\n");
    fprintf(list->debug.dump.file, "<body>\n");

    writeListInfo(list, info);
    writeListData(list);

    fprintf(list->debug.dump.file, "<div style=\"overflow-x: auto; white-space: nowrap;\">\n");
    fprintf(list->debug.dump.file, "<img src=\"list_graph_%03d.svg\" "
            "style=\"zoom: 0.65; transform-origin: top left;\">\n",
            list->debug.dump.image_counter);
    fprintf(list->debug.dump.file, "</div>\n");

    fprintf(list->debug.dump.file, "<hr style=\"margin: 40px 0; border: 2px solid #ccc;\">\n");
    fprintf(list->debug.dump.file, "</body>\n");
    fprintf(list->debug.dump.file, "</html>\n");
}


void listDump(List* list, ListStatus status, const char* file,
              const char* function, int line, const char* format, ...)
{
    assert(list); assert(list->storage); assert(list->debug.dump.file);
    assert(format); assert(file); assert(function);

    char message[BUFFER_SIZE] = {};
    if (format[0] != '\0') {
        va_list args;
        va_start(args, format);
        vsnprintf(message, BUFFER_SIZE, format, args);
        va_end(args);
    }

    DumpInfo info = {message, status, file, function, line};
    char graph_dot_file[BUFFER_SIZE * 2] = {};
    char graph_svg_file[BUFFER_SIZE * 2] = {};
   
    snprintf(graph_dot_file, BUFFER_SIZE * 2, "%s/list_graph_%03d.dot",
             list->debug.dump.directory, list->debug.dump.image_counter);
    snprintf(graph_svg_file, BUFFER_SIZE * 2, "%s/list_graph_%03d.svg",
             list->debug.dump.directory, list->debug.dump.image_counter);

    generateGraph(list, graph_dot_file);
    convertDotToSvg(graph_dot_file, graph_svg_file); 

    char command[BUFFER_SIZE * 3] = {};
    snprintf(command, BUFFER_SIZE * 3, "rm %s", graph_dot_file);
//    system(command);
   
    createHtmlDump(list, &info, graph_svg_file);

    list->debug.dump.image_counter++;
}
