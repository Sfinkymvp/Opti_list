#ifndef _HTML_DUMP_H_
#define _HTML_DUMP_H_


#include "data.h"


void listDump(List* list, ListStatus status, const char* file,
              const char* function, int line, const char* format, ...);


#endif // _HTML_DUMP_H_
