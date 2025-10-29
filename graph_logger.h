#ifndef _GRAPH_LOGGER_H_
#define _GRAPH_LOGGER_H_


#include "data.h"


extern const char* IMAGE_DIRECTORY;


const int BUFFER_SIZE = 256;


void listDump(List* list, const char* filename,
              const char* function_name, int line_number);


#endif // _GRAPH_LOGGER_H_
