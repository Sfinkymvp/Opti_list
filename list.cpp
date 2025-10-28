#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include "data.h"
#include "list.h"


static void initializeFreeNodes(List* list, int start_index, int last_index)
{
    assert(list != NULL);
    assert(list->storage != NULL);

    for (int index = start_index; index < last_index; index++) {
        list->storage[index].next = index + 1;
#ifdef DEBUG
        list->storage[index].value = LIST_POISON_VALUE;
#else 
        list->storage[index].value = 0;
#endif // DEBUG
        list->storage[index].prev = END_OF_MEMORY;
    }

    list->storage[last_index - 1].next = END_OF_MEMORY;
}


static bool isNodeFree(List* list, int index)
{
    assert(list != NULL);
    assert(list->storage != NULL);
    assert(index >= 0 && index < list->capacity);

    return list->storage[index].prev == END_OF_MEMORY;
}


static bool isValidIndex(List* list, int index)
{
    assert(list != NULL);
    assert(list->storage != NULL);

    return index >= 0 && index < list->capacity && !isNodeFree(list, index);
}


ListStatus listInsertBefore(List* list, int index, DataType value)
{
    assert(list != NULL);
    assert(list->storage != NULL);

    CHECK_STATUS(listExpand(list));

    if (!isValidIndex(list, index - 1))
        return LIST_INVALID_INDEX;

    int new_index = list->free_head;
    list->free_head = list->storage[list->free_head].next;

    list->storage[new_index].value = value;

    list->storage[new_index].next = list->storage[index - 1].next;
    list->storage[list->storage[new_index].next].prev = new_index;

    list->storage[index - 1].next = new_index;
    list->storage[new_index].prev = index - 1;

    return LIST_OK;    
}


ListStatus listInsertAfter(List* list, int index, DataType value)
{
    assert(list != NULL);
    assert(list->storage != NULL);

    CHECK_STATUS(listExpand(list));

    if (!isValidIndex(list, index))
        return LIST_INVALID_INDEX;

    int new_index = list->free_head;
    list->free_head = list->storage[list->free_head].next;

    list->storage[new_index].value = value;

    list->storage[new_index].next = list->storage[index].next;
    list->storage[list->storage[new_index].next].prev = new_index;

    list->storage[index].next = new_index;
    list->storage[new_index].prev = index;

    return LIST_OK;    
}


ListStatus listDelete(List* list, int index)
{
    assert(list != NULL);
    assert(list->storage != NULL);

    if (!isValidIndex(list, index) || index == 0)
        return LIST_INVALID_INDEX;

#ifdef DEBUG
    list->storage[index].value = LIST_POISON_VALUE;
#else
    list->storage[index].value = 0;
#endif // DEBUG
    list->storage[list->storage[index].next].prev = list->storage[index].prev;
    list->storage[list->storage[index].prev].next = list->storage[index].next;

    list->storage[index].prev = END_OF_MEMORY;
    list->storage[index].next = list->free_head;
    list->free_head = index;

    return LIST_OK;  
}


ListStatus listExpand(List* list)
{
    assert(list != NULL);
    assert(list->storage != NULL);

    if (list->free_head != END_OF_MEMORY)
        return LIST_OK;

    void* temp = realloc(list->storage, 2 * list->capacity * sizeof(Node));
    if (temp == NULL)
        return LIST_OUT_OF_MEMORY;

    list->storage = (Node*)temp;
    list->free_head = list->capacity;
    list->capacity *= 2;

    initializeFreeNodes(list, list->capacity / 2, list->capacity);

    return LIST_OK;
}


ListStatus listConstructor(List* list)
{
    assert(list != NULL);

    void* temp = calloc(START_CAPACITY, sizeof(Node));
    if (temp == NULL)
        return LIST_OUT_OF_MEMORY;

    list->storage = (Node*)temp;
    list->capacity = START_CAPACITY;
    list->free_head = 1;

    list->storage[0].value = LIST_DUMMY_VALUE;
    list->storage[0].prev = 0;
    list->storage[0].next = 0;
    initializeFreeNodes(list, list->free_head, list->capacity);

    return LIST_OK;   
}


void listDestructor(List* list)
{
    assert(list != NULL);
    
    free(list->storage);
    list->storage = NULL;
    list->capacity = 0;
    list->free_head = END_OF_MEMORY;
}

