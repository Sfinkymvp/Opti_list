#ifndef _LIST_H_
#define _LIST_H_


#include "data.h"
#include "graph_logger.h"


#define LIST_DUMP(name, message, status)                                   \
    do {                                                                   \
        listDump(name, message, status, __FILE__, __func__, __LINE__);     \
    } while (0)


#define LIST_VERIFY(list, message)                     \
    do {                                               \
        ListStatus __status = listVerify(list);        \
        LIST_DUMP(list, message, __status);        \
        if (__status != LIST_OK) {                     \
            return __status;                           \
        }                                              \
    } while (0)


ListStatus listInsertBefore(List* list, int index, DataType value);


ListStatus listInsertAfter(List* list, int index, DataType value);


ListStatus listDelete(List* list, int index);


ListStatus listExpand(List* list);


ListStatus listConstructor(List* list);


void listDestructor(List* list);


#endif // _LIST_H_
