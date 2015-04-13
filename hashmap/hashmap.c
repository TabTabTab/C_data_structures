//hashmap.c

#include <stdio.h>
#include <stdlib.h>
#include "hashmap.h"
#define STARTSIZE (8)
#define EXPANDQUOTA (0.75)
#define INCREMENTFACTOR (2)

typedef struct node_t node_t;
struct node_t {
        node_t* next;
        void* key;
        void* value;
};
struct hashmap_t {
        size_t size;
        size_t capacity;
        node_t* elements;
        equals_callback_t equality;
        hashcode_callback_t hashcode;
};

static void map_maintain(hashmap_t* map);
static void map_freenode(node_t* n);
static void map_insertnode(hashmap_t* map, node_t* n);
static hashmap_t* xcalloc_hashmap();
static node_t* xcalloc_node();
static node_t* xcalloc_elements();
hashmap_t* new_map(equals_callback_t equals_callback, hashcode_callback_t hashcode_callback)
{
        return new_map_with_init_capacity(equals_callback,hashcode_callback,STARTSIZE);
}
hashmap_t* new_map_with_init_capacity(equals_callback_t equals_callback,hashcode_callback_t hashcode_callback, size_t initial_capacity)
{
        size_t capacity= initial_capacity > STARTSIZE? initial_capacity : STARTSIZE;
        hashmap_t* map;
        map=xcalloc_hashmap();
        map->equality=equals_callback;
        map->hashcode=hashcode_callback;
        map->capacity=capacity;
        map->elements=xcalloc_elements(capacity);
        return map;
}
void map_keys(hashmap_t* map, void** key_list)
{
        void** current_index=key_list;
        node_t* temp_node;
        for(size_t i=0; i<map->capacity;i++){
                temp_node=map->elements[i].next;
                while(temp_node){
                        node_t* nextnode=temp_node->next;
                        *current_index=temp_node->key;
                        temp_node=temp_node->next;
                        current_index++;
                }
        }
}
void map_values(hashmap_t* map, void** value_list)
{
        void** current_index=value_list;
        node_t* temp_node;
        for(size_t i=0; i<map->capacity;i++){
                temp_node=map->elements[i].next;
                while(temp_node){
                        node_t* nextnode=temp_node->next;
                        *current_index=temp_node->value;
                        temp_node=temp_node->next;
                        current_index++;
                }
        }
}
void* map_insert(hashmap_t* map, void* key, void* value)
{       
        int hash_val=map->hashcode(key)%map->capacity;
        node_t* temp_node;
        temp_node = &map->elements[hash_val];
        while(temp_node->next){
                if(map->equality(temp_node->next->key,key)){
                        void* old_value=temp_node->next->value;
                        temp_node->next->value=value;
                        
                        return old_value;
                }
                temp_node=temp_node->next;
        }
        node_t* entry;
        entry=xcalloc_node();
        entry->key=key;
        entry->value=value;
        temp_node->next=entry;
        map->size++;
        map_maintain(map);
        return NULL;
}
void* map_remove(hashmap_t* map, void* key)
{
        if(!map_contains_key(map,key)){
                return NULL;
        }
        int hashval=map->hashcode(key)%map->capacity;
        node_t* temp_node;
        temp_node=&map->elements[hashval];
        while(temp_node->next){
                void* nodekey=temp_node->next->key;
                if(map->equality(key,nodekey)){
                        node_t* discarded=temp_node->next;
                        void* value=discarded->value;
                        temp_node->next=discarded->next;
                        free(discarded);
                        map->size--;
                        return value;
                }
                temp_node=temp_node->next;
        }
        return NULL;
        
}
void* map_get(hashmap_t* map, void* key)
{
        int hashval=map->hashcode(key)%map->capacity;
        node_t* temp_node;
        temp_node=map->elements[hashval].next;
        while(temp_node){
                void* node_key=temp_node->key;
                if(map->equality(key,node_key)){
                        return temp_node->value;
                }
                temp_node=temp_node->next;
        }
        return NULL;
}
int map_contains_key(hashmap_t* map, void* key)
{
        int hashval=map->hashcode(key)%map->capacity;
        node_t* temp_node;
        temp_node=map->elements[hashval].next;
        int exists=0;
        int i=0;
        while(temp_node){
                void* nodekey=temp_node->key;
                if(map->equality(key,nodekey)){
                        exists=1;
                        break;
                }
                temp_node=temp_node->next;
                i++;
        }
        return exists;
}
size_t map_size(hashmap_t* map)
{
        return map->size;
}
int map_is_empty(hashmap_t* map)
{
        return 0==map->size?1:0;
}
void map_free(hashmap_t* map)
{
        //firstly free all nodes
        node_t* temp_node;
        for(size_t i=0; i<map->capacity;i++){
                temp_node=map->elements[i].next;
                while(temp_node){
                        node_t* nextnode=temp_node->next;
                        free(temp_node);
                        temp_node=nextnode;
                }
        }
        free(map->elements);
        free(map);
}
size_t map_capacity(hashmap_t* map)
{
        return map->capacity;
}
//static functions begin
static hashmap_t* xcalloc_hashmap(){
        hashmap_t* map;
        map=calloc(1,sizeof(struct hashmap_t));
        if(map==NULL){
                fprintf(stderr,"out of memory");
                exit(1);
        }
        return map;
}
static node_t* xcalloc_elements(size_t size){
        node_t* elements=calloc(size,sizeof(struct node_t));
        if(elements==NULL){
                fprintf(stderr,"out of memory");
                exit(1);
        }
        return elements;
}
static node_t* xcalloc_node(){
        node_t* n=calloc(1,sizeof(struct node_t));
        if(n==NULL){
                fprintf(stderr,"out of memory");
                exit(1);
        }
        return n;
}
static void map_maintain(hashmap_t* map)
{
        int currentsize=map->size;
        double currentquota=((double)currentsize)/map->capacity;
        if(currentquota<EXPANDQUOTA){
                return;
        }
        node_t* elements=map->elements;
        map->elements=xcalloc_elements(map->capacity*INCREMENTFACTOR);
        node_t* temp_node;
        size_t capacity=map->capacity;
        map->capacity*=INCREMENTFACTOR;
        
        map->size=0;
        for(size_t i=0; i<capacity;i++){
                temp_node=elements[i].next;
                while(temp_node){
                        node_t* nextnode=temp_node->next;
                        map_insertnode(map,temp_node);
                        temp_node=nextnode;
                }
        }
        map_freenode(elements);
}
static int map_check(hashmap_t* map)
{
        return 1;
}
static void map_freenode(node_t* n)
{
        free(n);
}
/*
*A naive push used by @maintain when map size is increased
*changes values of node_t.next
*/
static void map_insertnode(hashmap_t* map, node_t* n)
{
        n->next=0;
        int hashval=map->hashcode(n->key)%map->capacity;
        
        node_t* temp_node;
        temp_node = &map->elements[hashval];
        while(temp_node->next){
                temp_node=temp_node->next;
        }
        temp_node->next=n;
        map->size++;
}

//static functions end

//required test methods begins
/*
int equals (void* c1, void* c2) {
        char* first=(char*)c1;
        char* second=(char*)c2;
        int compvalue= (*first) == (*second);
        return compvalue;
}
static int hashfunc(void* key)
{
        //for char
        char* c=(char*)key;
        int hashcode=*c;
        return hashcode;
}

//TESTCODE BEGIN
static void test_push_with_kchar_vint(){
        char c='c';
        char f='v';
        char k='k';
        char g='g';
        char h='h';
        char c2='c';
        int v1=12,v2=13,v3=14,v4=15,v5=22;
        hashmap_t* map=new_map(equals,hashfunc);
        if(NULL!=map_insert(map,&c,&v1)){
                fprintf(stderr,"test_push_with_kchar_vint t1 fail\n");
                exit(1);
        }
        if(NULL!=map_insert(map,&f,&v2)){
                fprintf(stderr,"test_push_with_kchar_vint t2 fail\n");
                exit(1);
        }
        if(NULL!=map_insert(map,&k,&v3)){
                fprintf(stderr,"test_push_with_kchar_vint t3 fail\n");
                exit(1);
        }
        if(3!=map_size(map)){
                fprintf(stderr,"test_push_with_kchar_vint t4 fail\n");
                exit(1);  
        }
        if(&v1!=map_insert(map,&c2,&v4)){
                fprintf(stderr,"test_push_with_kchar_vint t5 fail\n");
                exit(1);
        }
        if(3!=map_size(map)){
                fprintf(stderr,"test_push_with_kchar_vint t6 fail\n");
                exit(1);  
        }
        if(&v4!=map_get(map,&c)){
                fprintf(stderr,"test_push_with_kchar_vint t7 fail\n");
                exit(1);
        }
        map_free(map);
        
}
static void test_remove_with_kchar_vint(){
        char c='c';
        char f='v';
        char k='k';
        char g='g';
        char h='h';
        char c2='c';
        int v1=12,v2=13,v3=14,v4=15,v5=22;
        hashmap_t* map=new_map(equals,hashfunc);
        if(NULL!=map_insert(map,&c,&v1)){
                fprintf(stderr,"test_push_with_kchar_vint t1 fail\n");
                exit(1);
        }
        if(NULL!=map_insert(map,&f,&v2)){
                fprintf(stderr,"test_push_with_kchar_vint t2 fail\n");
                exit(1);
        }
        if(NULL!=map_insert(map,&k,&v3)){
                fprintf(stderr,"test_push_with_kchar_vint t3 fail\n");
                exit(1);
        }
        if(3!=map_size(map)){
                fprintf(stderr,"test_push_with_kchar_vint t4 fail\n");
                exit(1);  
        }
        if(&v1!=map_insert(map,&c2,&v4)){
                fprintf(stderr,"test_push_with_kchar_vint t5 fail\n");
                exit(1);
        }
        if(3!=map_size(map)){
                fprintf(stderr,"test_push_with_kchar_vint t6 fail\n");
                exit(1);  
        }
        if(&v4!=map_get(map,&c)){
                fprintf(stderr,"test_push_with_kchar_vint t7 fail\n");
                exit(1);
        }
        map_free(map);
        
}
//TESTCODE END
/*
int main(){
        test_push_with_kchar_vint();
        //printf("hello\n");
        //node_t temp_node;
        /*
        char c='c';
        char f='v';
        char k='k';
        char g='g';
        char h='h';
        char c2='c';
        int v1=12,v2=13,v3=14,v4=15;
        hashmap_t* map=new_map(equals,hashfunc);
        map_insert(map,&c,&v1);
        map_contains_key(map,&c);
        map_insert(map,&f,&v2);
        map_insert(map,&k,&v3);
        map_insert(map,&c2,&v3);
        map_insert(map,&h,&v4);
        map_remove(map,&k);
        void** keys=calloc(map_size(map),sizeof(void*));
        
        map_keys(map,keys);
        for(int i=0;i<map_size(map);i++){
                //char** cpp=(char**)keys;
                //printf("I got the cpp\n");
                char* c=keys[i];
                //printf("c*\n");
                char c2=*c;
                //printf("gag\n");
                printf("key: %c\n",c2);
                //printf("got\n");
        } 
        void** vals=calloc(map_size(map),sizeof(void*));
        
        map_values(map,vals);
        for(int i=0;i<map_size(map);i++){
                //char** cpp=(char**)keys;
                //printf("I got the cpp\n");
                int* c=vals[i];
                //printf("c*\n");
                int c2=*c;
                //printf("gag\n");
                printf("val: %d\n",c2);
                //printf("got\n");
        } 
        free(keys);
        free(vals);
        
        //printf(
        
        
        /* for(int i=0;i<map_size(map);i++){
                char** cpp=(char**)keys;
                printf("key after: %c\n",**cpp);
                keys++;
        }
        printf("size: %u\n",map_size(map));
        map_free(map);
        /* printf("does it contain an c?: %d\n",contains(map, &c));
        printf("does it contain an g?: %d\n",contains(map, &g));
        printf("does it contain an h?: %d\n",contains(map, &h));
        printf("does it contain a f?: %d\n",contains(map, &f)); */
  /*      printf("HashSap testcases success\n");
}
*/
//test code ends
