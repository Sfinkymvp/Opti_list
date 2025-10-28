#include <stdio.h>
#include <assert.h>


#include "list.h"
#include "graph_logger.h"


int main()
{
#ifdef DEBUG
    LIST_INIT(list);
#else
    List list = {};
#endif // DEBUG

    CHECK_OR_RETURN(listConstructor(&list), &list);
    LIST_DUMP(&list, 1);
    CHECK_OR_RETURN(listInsertBefore(&list, 1, 52), &list);
    LIST_DUMP(&list, 2);
    CHECK_OR_RETURN(listInsertBefore(&list, 2, 42), &list);
    LIST_DUMP(&list, 3);
    CHECK_OR_RETURN(listInsertBefore(&list, 2, 11), &list);
    LIST_DUMP(&list, 4);
    CHECK_OR_RETURN(listInsertBefore(&list, 2, 121), &list);
    LIST_DUMP(&list, 5);
    CHECK_OR_RETURN(listInsertBefore(&list, 1, 228), &list);
    LIST_DUMP(&list, 6);
    CHECK_OR_RETURN(listInsertBefore(&list, 1, 228), &list);
    LIST_DUMP(&list, 7);
    CHECK_OR_RETURN(listDelete(&list, 1), &list);
    LIST_DUMP(&list, 8);

    listDestructor(&list);
    return 0;
}
