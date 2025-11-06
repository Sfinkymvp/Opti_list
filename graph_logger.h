#ifndef _GRAPH_LOGGER_H_
#define _GRAPH_LOGGER_H_


#include "data.h"


void listDump(List* list, const char* message, ListStatus status,
              const char* file, const char* function, int line);

             
#endif // _GRAPH_LOGGER_H_
