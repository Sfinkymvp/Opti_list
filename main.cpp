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
    CHECK_OR_RETURN(listInsertBefore(&list, 1, 52), &list);
    CHECK_OR_RETURN(listInsertBefore(&list, 2, 42), &list);
    CHECK_OR_RETURN(listInsertBefore(&list, 3, 11), &list);
    CHECK_OR_RETURN(listDelete(&list, 1), &list);
    CHECK_OR_RETURN(listInsertAfter(&list, 2, 121), &list);
    CHECK_OR_RETURN(listInsertBefore(&list, 1, 228), &list);
    CHECK_OR_RETURN(listInsertAfter(&list, 4, 228), &list);
    //list.storage[6].prev = -1100;
    CHECK_OR_RETURN(listDelete(&list, 5), &list);
    CHECK_OR_RETURN(listInsertBefore(&list, 5, 1773), &list);
    //list.storage[4].next = 2412;
    CHECK_OR_RETURN(listInsertAfter(&list, 5, 112), &list);
    list.storage[2].prev = 4234;
    list.storage[4].prev = -120;
    CHECK_OR_RETURN(listInsertBefore(&list, 6, 322), &list);
    //CHECK_OR_RETURN(listInsertBefore(&list, 3, 12342), &list);
    //CHECK_OR_RETURN(listInsertBefore(&list, 1, -1233), &list);
    //list.storage[3].next = 1;
    //list.storage[6].prev = -666;

    listDestructor(&list);
    return 0;
}
