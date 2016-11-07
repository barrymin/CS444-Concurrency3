#include <list>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t numsearch = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t numinsert = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t insertlock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t noInsert = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t noSearch = PTHREAD_MUTEX_INITIALIZER;
long searchers=0,inserters=0;
std::list<long> llist;

void *searcher(void*);
void *inserter(void*);
void *deleter(void*);
long list_search(long);
void list_insert(long);
void list_delete(long);
void print_list();
long generate_random_num(long,long);

int main(){
    srand(time(NULL));
    //dispatching threads look
    while(true){
        long operation = generate_random_num(1,3);
        long n = generate_random_num(1,10);
        long sleeptime = generate_random_num(0,4);
        pthread_t thread;
        if(operation==1){
            pthread_create(&thread, NULL, searcher,(void *)n);
        }else if(operation==2){
            pthread_create(&thread, NULL, inserter,(void *)n);
            //if(n<=3)
                //pthread_create(&thread, NULL, searcher,(void *)n);
        }else{
            pthread_create(&thread, NULL, deleter,(void *)n);
        }
        sleep(sleeptime);
    }
    return 0;
}

void * searcher(void * n){
    pthread_mutex_lock(&numsearch);
    ++searchers;
    if(searchers==1)
        pthread_mutex_lock(&noSearch);
    pthread_mutex_unlock(&numsearch);

    printf("Searching for %d..\n",n);
    if(list_search((long)n))
        printf("Found %d..\n",(long)n);
    else
        printf("Could'n find %d\n",(long)n);
    pthread_mutex_lock(&numsearch);
    --searchers;
    if(searchers==0)
        pthread_mutex_unlock(&noSearch);
    pthread_mutex_unlock(&numsearch);
    printf("Searcher exiting..\n");
    pthread_exit(NULL);
}

void * inserter(void * n){
    pthread_mutex_lock(&numinsert);
       // printf("Inserting increment %d..\n",n);

    ++inserters;
    if(inserters==1)
        pthread_mutex_lock(&noInsert);
    pthread_mutex_unlock(&numinsert);

    pthread_mutex_lock(&insertlock);
       // printf("Inserting about to %d..\n",n);
    printf("Inserting %d..\n",n);
    list_insert((long)n);
    print_list();
    pthread_mutex_unlock(&insertlock);

    pthread_mutex_lock(&numinsert);
        //printf("Inserting dec %d..\n",n);
    --inserters;
    if(inserters==0)
        pthread_mutex_unlock(&noInsert);
    pthread_mutex_unlock(&numinsert);
    printf("Inserter exiting..\n");
    pthread_exit(NULL);

}

void * deleter(void * n){
    pthread_mutex_lock(&noInsert);
    pthread_mutex_lock(&noSearch);
    printf("Deleting %d..\n",n);
    list_delete((long) n);
    print_list();
    pthread_mutex_unlock(&noInsert);
    pthread_mutex_unlock(&noSearch);
    printf("Deleter exiting..\n");
    pthread_exit(NULL);
}
long list_search(long n){
    for (std::list<long>::iterator it=llist.begin(); it!=llist.end(); ++it){
        if(n==*it)
            return 1;
    }
    return 0;
}

void list_insert(long n){
    llist.push_back (n);
}

void list_delete(long n){
    llist.remove(n);
}

void print_list(){
    for (std::list<long>::iterator it=llist.begin(); it!=llist.end(); ++it)
        printf("%d ",*it);
    printf("\n");
}


long generate_random_num(long a,long b){
	return rand() % b + a;
}
