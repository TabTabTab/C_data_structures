// singlelinkedlist.h

typedef struct list_t list_t;

void list_add_last(list_t** list, void* element);
void list_free(list_t** list);
void* list_get_first(list_t** list);
void list_reverse(list_t** list);
void append_lists(list_t** list1,list_t** list2);
void list_apply(list_t** list, void (*func)(void*));