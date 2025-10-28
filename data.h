#ifndef _DATA_H_
#define _DATA_H_


const int END_OF_MEMORY = -1;
const int START_CAPACITY = 8;
const int BUFFER_SIZE = 256;


#ifdef DEBUG

#define LIST_INIT(name) List name = {.debug_info = \
                        {#name, __FILE__, __func__, __LINE__}};


#define LIST_DUMP(name, number)                              \
    do {                                                     \
        listDump(name, number, __FILE__, __func__, __LINE__);\
    } while (0)
 
#endif // DEBUG


#define CHECK_STATUS(function_call)               \
    do {                                          \
        ListStatus _status = function_call;       \
        if (_status != LIST_OK)                   \
            return _status;                       \
    } while (0)


#define CHECK_OR_RETURN(function_call, list_ptr)  \
    do {                                          \
        ListStatus _status = function_call;       \
        if (_status != LIST_OK) {                 \
            fprintf(stderr, "Error!\n");          \
            listDestructor(list_ptr);             \
            return _status;                       \
        }                                         \
    } while (0)


typedef enum {
    LIST_OK = 0,
    LIST_INVALID_INDEX,
    LIST_OUT_OF_MEMORY
} ListStatus;


typedef int DataType;
#define DATATYPE_SPEC "%d"
const DataType LIST_POISON_VALUE = 0xABADBABE;
const DataType LIST_DUMMY_VALUE = 0xAB0BA228;


#ifdef DEBUG
typedef struct {
    const char* list_name;
    const char* source_file;
    const char* function_name;
    int line_number;
} ListDebugInfo;


typedef struct {
    const char* source_file;
    const char* function_name;
    int line_number;
    int file_number;
} DumpInfo;
#endif // DEBUG


typedef struct {
    int prev;
    DataType value;
    int next;
} Node;


typedef struct {
    Node* storage;
    int capacity;
    int free_head;
#ifdef DEBUG
    ListDebugInfo debug_info;
#endif // DEBUG
} List;


#endif // _DATA_H_
