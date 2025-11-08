#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include "data.h"
#include "list.h"


static void initializeFreeNodes(List* list, int start_index, int last_index)
{
    assert(list); assert(list->storage);

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
    assert(list); assert(list->storage);
    assert(index >= 0 && index < list->capacity);

    return list->storage[index].prev == EMPTY;
}


static bool isValidIndex(List* list, int index)
{
    assert(list); assert(list->storage);

    return index >= 0 && index < list->capacity && !isNodeFree(list, index);
}


ListStatus listVerify(List* list)
{
    assert(list); assert(list->storage);

    if (list->capacity <= 0)
        return LIST_CAPACITY_INVALID;
    if (list->free_head != EMPTY && (list->free_head < 0 || list->free_head >= list->capacity))
        return LIST_FREE_HEAD_INVALID;
    if (list->storage[0].value != LIST_FICTIVE_VALUE)
        return LIST_ZERO_NODE_CORRUPTED;
    
    int count_element = 0;    
    for (int index = list->storage[0].next, count = 0; index != 0;
         index = list->storage[index].next, count++) {
        if (count >= list->capacity)
            return LIST_LOOP_DETECTED;
        if (index < 0 || index >= list->capacity)
            return LIST_INDEX_OUT_OF_BOUNDS;
        if (isNodeFree(list, index))
            return LIST_FREE_NODE_MIXED;
        if (list->storage[index].prev < 0 && list->storage[index].prev != EMPTY
            || list->storage[index].prev >= list->capacity)
            return LIST_INDEX_OUT_OF_BOUNDS;
        if (list->storage[index].next < 0 && list->storage[index].next != EMPTY
            || list->storage[index].next >= list->capacity)
            return LIST_INDEX_OUT_OF_BOUNDS;
        if (list->storage[list->storage[index].next].prev != index)
            return LIST_ERR_PREV_NEXT;
        count_element++;
    }
    if (list->size != count_element)
        return LIST_SIZE_INVALID;

    for (int index = list->free_head, count = 0; index != EMPTY;
         index = list->storage[index].next, count++) {
        if (count >= list->capacity)
            return LIST_LOOP_DETECTED;
        if (index < 0 || index >= list->capacity)
            return LIST_INDEX_OUT_OF_BOUNDS;
        if (!isNodeFree(list, index))
            return LIST_FREE_STRUCTURE_BROKEN;
        if (list->storage[index].prev < 0 && list->storage[index].prev != EMPTY
            || list->storage[index].prev >= list->capacity)
            return LIST_INDEX_OUT_OF_BOUNDS;
        if (list->storage[index].next < 0 && list->storage[index].next != EMPTY
            || list->storage[index].next >= list->capacity)
            return LIST_INDEX_OUT_OF_BOUNDS;
        if (list->storage[index].value != LIST_POISON_VALUE ||
            list->storage[index].prev != EMPTY)
            return LIST_VALUE_INVALID;
    }
   
    return LIST_OK;
}


ListStatus listInsertBefore(List* list, int index, DataType value)
{
    LIST_VERIFY(list, "Before inserting an element with value \'%d\' before index \'%d\'",
                value, index);

    if (!isValidIndex(list, index) || index == 0)
        return LIST_INDEX_OUT_OF_BOUNDS;

    int new_index = list->free_head;
    list->free_head = list->storage[list->free_head].next;

    list->storage[new_index].value = value;

    list->storage[new_index].next = index;
    list->storage[new_index].prev = list->storage[index].prev;

    list->storage[list->storage[index].prev].next = new_index;
    list->storage[index].prev = new_index;
    list->size++;

    CHECK_STATUS(listResize(list));

    LIST_VERIFY(list, "After inserting an element before index \'%d\', insertion index \'%d\'",
                index, new_index);

    return LIST_OK;    
}


ListStatus listInsertAfter(List* list, int index, DataType value)
{
    LIST_VERIFY(list, "Before inserting an element with value \'%d\' after index \'%d\'",
                value, index);

    if (!isValidIndex(list, index))
        return LIST_INDEX_OUT_OF_BOUNDS;

    int new_index = list->free_head;
    list->free_head = list->storage[list->free_head].next;

    list->storage[new_index].value = value;

    list->storage[new_index].next = list->storage[index].next;
    list->storage[list->storage[new_index].next].prev = new_index;

    list->storage[index].next = new_index;
    list->storage[new_index].prev = index;
    list->size++;

    CHECK_STATUS(listResize(list));

    LIST_VERIFY(list, "After inserting an element after index \'%d\', insertion index \'%d\'",
                index, new_index);

    return LIST_OK;    
}


ListStatus listDelete(List* list, int index)
{
    LIST_VERIFY(list, "Before delete an element at index \'%d\'", index);

    if (!isValidIndex(list, index) || index == 0)
        return LIST_INDEX_OUT_OF_BOUNDS;

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
    list->size--;

    LIST_VERIFY(list, "After delete an element at index \'%d\'", index);
    
    return LIST_OK;  
}


ListStatus listResize(List* list)
{
    assert(list);

    if (list->free_head != EMPTY)
        return LIST_OK;
    
    LIST_VERIFY(list, "Before list resize, capacity \'%d\'", list->capacity);

    void* temp = realloc(list->storage, 2 * list->capacity * sizeof(Node));
    if (temp == NULL)
        return LIST_OUT_OF_MEMORY;

    list->storage = (Node*)temp;
    list->free_head = list->capacity;
    list->capacity *= 2;

    initializeFreeNodes(list, list->capacity / 2, list->capacity);
    
    LIST_VERIFY(list, "After list resize, capacity: \'%d\'", list->capacity);

    return LIST_OK;
}


static void listTransfer(List* list, Node* new_storage)
{
    assert(list); assert(list->storage);

    int new_index = 1;
    for (int old_index = list->storage[0].next; old_index != 0;
         old_index = list->storage[old_index].next, new_index++) {
        new_storage[new_index].value = list->storage[old_index].value;
        new_storage[new_index].prev = new_index - 1;
        if (new_index == list->size)
            new_storage[new_index].next = 0;
        else
            new_storage[new_index].next = new_index + 1;
    }

    for (int old_index = list->free_head; old_index != EMPTY;
         old_index = list->storage[old_index].next, new_index++) {
        new_storage[new_index].value = list->storage[old_index].value;
        new_storage[new_index].prev = EMPTY;
        if (list->storage[old_index].next == EMPTY)
            new_storage[new_index].next = EMPTY;
        else
            new_storage[new_index].next = new_index + 1;
    }
}


ListStatus listLinealizer(List* list)
{
    LIST_VERIFY(list, "Before list linearization");

    Node* new_storage = (Node*)calloc(list->capacity, sizeof(Node));
    if (new_storage == NULL)
        return LIST_OUT_OF_MEMORY;

    listTransfer(list, new_storage); 

    list->free_head = list->size + 1;
    new_storage[0].value = list->storage[0].value;
    if (list->size == 0) {
        new_storage[0].prev = 0;
        new_storage[0].next = 0;
    } else {
        new_storage[0].prev = list->size;
        new_storage[0].next = 1; 
    }

    free(list->storage);
    list->storage = new_storage;

    LIST_VERIFY(list, "After list linearization");

    return LIST_OK;
}


#ifdef DEBUG
static ListStatus openGraphDumpFile(List* list)
{
    assert(list);

    snprintf(list->debug.dump.directory, BUFFER_SIZE, "%s/list_dump_%03d",
             IMAGE_DIRECTORY, list->debug.dump.dump_counter);

    char command[BUFFER_SIZE * 3] = {};
    snprintf(command, BUFFER_SIZE * 3, "rm -rf %s && mkdir -p %s",
             list->debug.dump.directory, list->debug.dump.directory);
    system(command);

    char filename[BUFFER_SIZE * 2] = {};
    snprintf(filename, BUFFER_SIZE * 2, "%s/list_dump_%03d.html",
             list->debug.dump.directory, list->debug.dump.dump_counter);

    list->debug.dump.file = fopen(filename, "w");
    if (list->debug.dump.file == NULL)
        return LIST_ERR_DUMP_FILE_OPEN;

    return LIST_OK;
}
#endif // DEBUG


ListStatus listConstructor(List* list)
{
    assert(list);

    void* temp = calloc(START_CAPACITY, sizeof(Node));
    if (temp == NULL)
        return LIST_OUT_OF_MEMORY;

#ifdef DEBUG
    static int dump_counter = 1;
    list->debug.dump.dump_counter = dump_counter;
    list->debug.dump.image_counter = 1;
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

    LIST_VERIFY(list, "After initializing the list");

    return LIST_OK;   
}


void listDestructor(List* list)
{
    assert(list);
   
#ifdef DEBUG
    fclose(list->debug.dump.file);
#endif // DEBUG

    free(list->storage);
    list->storage = NULL;
    list->capacity = 0;
    list->free_head = EMPTY;
}
