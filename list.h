#ifndef _LIST_H_
#define _LIST_H_


#include <stdarg.h>


#include "data.h"
#include "graph_generator.h"
#include "html_dump.h"


#define LIST_INIT(name)                                   \
    List name = {};                                       \
    name.debug.creation = (ListCreationInfo){             \
        #name, __FILE__, __func__, __LINE__               \
    };


#define LIST_DUMP(name, status, format, ...)              \
    do {                                                  \
        listDump(name, status, __FILE__,                  \
        __func__, __LINE__, format, ##__VA_ARGS__);       \
    } while (0)


#define LIST_VERIFY(list, format, ...)                    \
    do {                                                  \
        ListStatus __status = listVerify(list);           \
        LIST_DUMP(list, __status, format, ##__VA_ARGS__); \
        if (__status != LIST_OK) {                        \
            return __status;                              \
        }                                                 \
    } while (0)


ListStatus listVerify(List* list);


ListStatus listInsertBefore(List* list, int index, DataType value);


ListStatus listInsertAfter(List* list, int index, DataType value);


ListStatus listDelete(List* list, int index);


ListStatus listResize(List* list);


ListStatus listConstructor(List* list);


void listDestructor(List* list);


#endif // _LIST_H_
