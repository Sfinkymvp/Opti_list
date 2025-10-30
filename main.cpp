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
    LIST_DUMP(&list);
    CHECK_OR_RETURN(listInsertBefore(&list, 1, 52), &list);
    LIST_DUMP(&list);
    CHECK_OR_RETURN(listInsertBefore(&list, 2, 42), &list);
    LIST_DUMP(&list);
    CHECK_OR_RETURN(listInsertBefore(&list, 3, 11), &list);
    LIST_DUMP(&list);
    CHECK_OR_RETURN(listDelete(&list, 1), &list);
    LIST_DUMP(&list);
    CHECK_OR_RETURN(listInsertAfter(&list, 2, 121), &list);
    LIST_DUMP(&list);
    CHECK_OR_RETURN(listInsertBefore(&list, 1, 228), &list);
    LIST_DUMP(&list);
    CHECK_OR_RETURN(listInsertAfter(&list, 4, 228), &list);
    //list.storage[6].prev = -1100;
    LIST_DUMP(&list);
    CHECK_OR_RETURN(listDelete(&list, 5), &list);
    LIST_DUMP(&list);
    CHECK_OR_RETURN(listInsertBefore(&list, 5, 1773), &list);
    //list.storage[4].next = 2412;
    LIST_DUMP(&list);
    CHECK_OR_RETURN(listInsertAfter(&list, 5, 112), &list);
    //list.storage[2].prev = -120;
    LIST_DUMP(&list);
    CHECK_OR_RETURN(listInsertBefore(&list, 6, 322), &list);
    LIST_DUMP(&list);
    list.storage[6].prev = -666;
    LIST_DUMP(&list);

    listDestructor(&list);
    return 0;
}
