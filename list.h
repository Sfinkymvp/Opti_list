#ifndef _LIST_H_
#define _LIST_H_


#include "data.h"


ListStatus listInsertBefore(List* list, int index, DataType value);


ListStatus listInsertAfter(List* list, int index, DataType value);


ListStatus listDelete(List* list, int index);


ListStatus listExpand(List* list);


ListStatus listConstructor(List* list);


void listDestructor(List* list);


#endif // _LIST_H_
