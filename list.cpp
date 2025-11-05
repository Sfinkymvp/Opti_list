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
        list->storage[index].prev = EMPTY;
    }

    list->storage[last_index - 1].next = EMPTY;
}


static bool isNodeFree(List* list, int index)
{
    assert(list != NULL);
    assert(list->storage != NULL);
    assert(index >= 0 && index < list->capacity);

    return list->storage[index].prev == EMPTY;
}


static bool isValidIndex(List* list, int index)
{
    assert(list != NULL);
    assert(list->storage != NULL);

    return index >= 0 && index < list->capacity && !isNodeFree(list, index);
}


static ListStatus listVerify(List* list)
{
    assert(list != NULL);
    assert(list->storage != NULL);

    if (list->capacity <= 0)
        return LIST_CORRUPTED;
    if (list->free_head != EMPTY && list->free_head < 0 || list->free_head >= list->capacity)
        return LIST_CORRUPTED;
    if (list->storage[0].value != LIST_FICTIVE_VALUE)
        return LIST_CORRUPTED;
    
    for (int index = list->storage[0].next; index != 0; index = list->storage[index].next) {
        if (index < 0 || index >= list->capacity)
            return LIST_CORRUPTED;
        if (isNodeFree(list, index))
            return LIST_CORRUPTED;
        if (list->storage[index].prev < 0 && list->storage[index].prev != EMPTY
            || list->storage[index].prev >= list->capacity)
            return LIST_CORRUPTED;
        if (list->storage[index].next < 0 && list->storage[index].next != EMPTY
            || list->storage[index].next >= list->capacity)
            return LIST_CORRUPTED;
        if (list->storage[list->storage[index].next].prev != index)
            return LIST_CORRUPTED;
    }

    for (int index = list->free_head; index != EMPTY; index = list->storage[index].next) {
        if (index < 0 || index >= list->capacity)
            return LIST_CORRUPTED;
        if (!isNodeFree(list, index))
            return LIST_CORRUPTED;
        if (list->storage[index].prev < 0 && list->storage[index].prev != EMPTY
            || list->storage[index].prev >= list->capacity)
            return LIST_CORRUPTED;
        if (list->storage[index].next < 0 && list->storage[index].next != EMPTY
            || list->storage[index].next >= list->capacity)
            return LIST_CORRUPTED;
        if (list->storage[index].value != LIST_POISON_VALUE ||
            list->storage[index].prev != EMPTY)
            return LIST_CORRUPTED;
    }
   
    return LIST_OK;
}


ListStatus listInsertBefore(List* list, int index, DataType value)
{
    CHECK_STATUS(listVerify(list));

    if (!isValidIndex(list, index - 1))
        return LIST_INVALID_INDEX;

    int new_index = list->free_head;
    list->free_head = list->storage[list->free_head].next;

    list->storage[new_index].value = value;

    list->storage[new_index].next = list->storage[index - 1].next;
    list->storage[list->storage[new_index].next].prev = new_index;

    list->storage[index - 1].next = new_index;
    list->storage[new_index].prev = index - 1;

    CHECK_STATUS(listExpand(list));

    CHECK_STATUS(listVerify(list));

    list->size++;
    return LIST_OK;    
}


ListStatus listInsertAfter(List* list, int index, DataType value)
{
    CHECK_STATUS(listVerify(list));

    if (!isValidIndex(list, index))
        return LIST_INVALID_INDEX;

    int new_index = list->free_head;
    list->free_head = list->storage[list->free_head].next;

    list->storage[new_index].value = value;

    list->storage[new_index].next = list->storage[index].next;
    list->storage[list->storage[new_index].next].prev = new_index;

    list->storage[index].next = new_index;
    list->storage[new_index].prev = index;

    CHECK_STATUS(listExpand(list));

    CHECK_STATUS(listVerify(list));

    list->size++;
    return LIST_OK;    
}


ListStatus listDelete(List* list, int index)
{
    CHECK_STATUS(listVerify(list));

    if (!isValidIndex(list, index) || index == 0)
        return LIST_INVALID_INDEX;

#ifdef DEBUG
    list->storage[index].value = LIST_POISON_VALUE;
#else
    list->storage[index].value = 0;
#endif // DEBUG
    list->storage[list->storage[index].next].prev = list->storage[index].prev;
    list->storage[list->storage[index].prev].next = list->storage[index].next;

    list->storage[index].prev = EMPTY;
    list->storage[index].next = list->free_head;
    list->free_head = index;

    CHECK_STATUS(listVerify(list));

    list->size--;
    return LIST_OK;  
}


ListStatus listExpand(List* list)
{
    CHECK_STATUS(listVerify(list));

    if (list->free_head != EMPTY)
        return LIST_OK;
    
    void* temp = realloc(list->storage, 2 * list->capacity * sizeof(Node));
    if (temp == NULL)
        return LIST_OUT_OF_MEMORY;

    list->storage = (Node*)temp;
    list->free_head = list->capacity;
    list->capacity *= 2;

    initializeFreeNodes(list, list->capacity / 2, list->capacity);
    
    CHECK_STATUS(listVerify(list));
    
    return LIST_OK;
}


#ifdef DEBUG
static ListStatus openGraphDumpFile(List* list)
{
    assert(list != NULL);

    snprintf(list->debug_info.dump.directory, BUFFER_SIZE, "%s/list_dump_%03d",
             IMAGE_DIRECTORY, list->debug_info.dump.dump_counter);

    char command[BUFFER_SIZE * 3] = {};
    snprintf(command, BUFFER_SIZE * 3, "rm -rf %s && mkdir -p %s",
             list->debug_info.dump.directory, list->debug_info.dump.directory);
    system(command);

    char filename[BUFFER_SIZE * 2] = {};
    snprintf(filename, BUFFER_SIZE * 2, "%s/list_dump_%03d.html",
             list->debug_info.dump.directory, list->debug_info.dump.dump_counter);

    list->debug_info.dump.file = fopen(filename, "w");
    if (list->debug_info.dump.file == NULL)
        return LIST_ERR_FILE_OPEN;

    return LIST_OK;
}
#endif // DEBUG


ListStatus listConstructor(List* list)
{
    assert(list != NULL);

    void* temp = calloc(START_CAPACITY, sizeof(Node));
    if (temp == NULL)
        return LIST_OUT_OF_MEMORY;

#ifdef DEBUG
    static int dump_counter = 1;
    list->debug_info.dump.dump_counter = dump_counter;
    dump_counter++;

    CHECK_STATUS(openGraphDumpFile(list));
#endif // DEBUG

    list->storage = (Node*)temp;
    list->capacity = START_CAPACITY;
    list->free_head = 1;

    list->storage[0].value = LIST_FICTIVE_VALUE;
    list->storage[0].prev = 0;
    list->storage[0].next = 0;
    list->size = 0;
    initializeFreeNodes(list, list->free_head, list->capacity);

    CHECK_STATUS(listVerify(list));

    return LIST_OK;   
}


void listDestructor(List* list)
{
    assert(list != NULL);
   
#ifdef DEBUG
    fclose(list->debug_info.dump.file);
#endif // DEBUG

    free(list->storage);
    list->storage = NULL;
    list->capacity = 0;
    list->free_head = EMPTY;
}
