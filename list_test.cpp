#include <stdio.h>
#include <stdlib.h>


#include "list_test.h"
#include "list.h"


int listTest1()
{
    LIST_INIT(list);
    CHECK_OR_RETURN(listConstructor(&list), &list);

    CHECK_OR_RETURN(listInsertAfter(&list, 0, 52), &list);
    CHECK_OR_RETURN(listInsertBefore(&list, 1, 42), &list);
    CHECK_OR_RETURN(listInsertBefore(&list, 2, 11), &list);
    CHECK_OR_RETURN(listDelete(&list, 1), &list);
    CHECK_OR_RETURN(listInsertAfter(&list, 2, 121), &list);
    CHECK_OR_RETURN(listInsertBefore(&list, 1, 228), &list);
    CHECK_OR_RETURN(listInsertAfter(&list, 4, 228), &list);
    CHECK_OR_RETURN(listDelete(&list, 5), &list);
    CHECK_OR_RETURN(listInsertBefore(&list, 4, 1773), &list);
    CHECK_OR_RETURN(listInsertAfter(&list, 4, 112), &list);
    CHECK_OR_RETURN(listLinealizer(&list), &list);
    CHECK_OR_RETURN(listInsertBefore(&list, 1, 322), &list);
    list.storage[2].prev = 155;
    list.storage[4].next = -120;
    
    LIST_VERIFY(&list, "Final dump");
    listDestructor(&list);
    return 0;
}


int listTest2()
{
    LIST_INIT(list);
    CHECK_OR_RETURN(listConstructor(&list), &list);
    
    CHECK_OR_RETURN(listInsertAfter(&list, 0, 1), &list);
    CHECK_OR_RETURN(listInsertAfter(&list, 1, 234), &list);
    CHECK_OR_RETURN(listInsertBefore(&list, 1, 753), &list);
    CHECK_OR_RETURN(listDelete(&list, 2), &list);
    CHECK_OR_RETURN(listLinealizer(&list), &list);
    CHECK_OR_RETURN(listInsertBefore(&list, 2, 344), &list);
    CHECK_OR_RETURN(listInsertAfter(&list, 0, -123), &list);
    CHECK_OR_RETURN(listInsertAfter(&list, 3, -894), &list);
    CHECK_OR_RETURN(listDelete(&list, 1), &list);
    CHECK_OR_RETURN(listLinealizer(&list), &list);
    list.storage[3].next = -125;
    list.storage[1].prev = -324;

    LIST_VERIFY(&list, "Final dump");
    listDestructor(&list);
    return 0;
}
