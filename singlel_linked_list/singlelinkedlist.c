//singlelinkedlist.c
#include <stdlib.h>
#include <stdio.h>
#include "singlelinkedlist.h"

struct list_t{
        void* data;
        list_t* next;
};
list_t* xcalloc_list()
{
        list_t* list;
        list=calloc(1,sizeof(struct list_t));
        if(NULL==list){
                fprintf(stderr,"out of memory\n");
        }
        return list;
}
void list_add_last(list_t** list, void* element)
{
        list_t* old_list;
        old_list=*list;
        list_t* new_item;
        new_item=xcalloc_list();
        new_item->data=element;
        new_item->next=NULL;
        if(NULL==old_list){
                //printf("added to NULL list\n");
                *list=new_item;
        }else{
                int itter=0;
                //printf("added to existing list\n");
                while(NULL!=old_list->next){
                        itter++;
                        old_list=old_list->next;
                }
                //printf("added after iteration: %d\n",itter);
                old_list->next=new_item;
        }
}
void list_free(list_t** list)
{
        list_t* old_list;
        old_list=*list;
/*         if(NULL==old_list){
                return;
        } */
        while(NULL!=old_list){
                list_t* next=old_list->next;
                free(old_list);
                old_list=next;
        }
}
void list_reverse(list_t** list)
{
        list_t* old_list;
        old_list=*list;
        if(NULL==old_list){
                return;
        }
        list_t* last_list=old_list;
        list_t* next_list=last_list->next;
        last_list->next=NULL;
        while(NULL!=next_list){
                list_t* next=next_list->next;
                next_list->next=last_list;
                last_list=next_list;
                next_list=next;
        }
        *list=last_list;
}
void append_lists(list_t** list1,list_t** list2)
{
        if(NULL==*list2){
                return;
        }
        list_t* current=*list2;
        while(NULL!=current){
               list_add_last(list1, current->data); 
                current=current->next;
        }

}
void list_apply(list_t** list, void (*func)(void*))
{
        if(NULL==*list){
                return;
        }
        list_t* current=*list;
        while(NULL!=current){
                func(current->data);
                current=current->next;
        }
}
void printlist(list_t** list)
{
        list_t* itter=*list;
        int index=0;
        while(itter!=NULL){
                char* c=(char*)itter->data;
                printf("value at index %d is: %c\n",index,*c);
                index++;
                itter=itter->next;
        }
}
static void print_list_item(void* item)
{
        printf("%c\n",*((char*)item));
}
int main(){
        list_t* list=NULL;
        list_t* list2=NULL;
        char a='a',b='b',c='c',d='d';
        list_add_last(&list,&a);
        list_add_last(&list,&b);
        list_add_last(&list2,&a);
        list_add_last(&list2,&b);
        //let's itterate!
        list_add_last(&list,&c);
/*         list_t* itter=list;
        int index=0;
        while(itter!=NULL){
                char* c=(char*)itter->data;
                printf("value at index %d is: %c\n",index,*c);
                index++;
                itter=itter->next;
        } */
        list_reverse(&list);
/*         itter=list;
        index=0;
        while(itter!=NULL){
                char* c=(char*)itter->data;
                printf("value at index %d is: %c\n",index,*c);
                index++;
                itter=itter->next;
        } */
        list_add_last(&list,&d);
/*         itter=list;
        index=0;
        while(itter!=NULL){
                char* c=(char*)itter->data;
                printf("value at index %d is: %c\n",index,*c);
                index++;
                itter=itter->next;
        } */
        append_lists(&list,&list2);
/*         itter=list;
        index=0;
        while(itter!=NULL){
                char* c=(char*)itter->data;
                printf("value at index %d is: %c\n",index,*c);
                index++;
                itter=itter->next;
        } */
        printlist(&list);
        list_apply(&list,print_list_item);
        list_free(&list);
        return 0;
        
}