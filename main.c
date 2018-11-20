#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>

#define MAX_LEVEL 10
#define BILLION 1000000000ULL

typedef struct skiplist * SKlist;
typedef struct levelist * Levels;
struct timespec start, end;
double elapsed_time;
FILE * close;
int dupli, notfound;
uint64_t pre_value;

typedef struct skiplist {
    uint64_t key;
    uint64_t value;
    struct skiplist ** forward;
} skiplist;

typedef struct levelist {
    int level;
    struct skiplist * header;
} levelist;

SKlist level_down(SKlist S, int posit, uint64_t key){
    while(S->forward[posit]->key < key)
        S = S->forward[posit];
    return S;
}
void Insert(Levels list, uint64_t key, uint64_t value) {
    int i, level=1;
    SKlist S = list->header;
    SKlist temp[MAX_LEVEL + 1];

    for (i = list->level; i >= 1; i--) temp[i] = level_down(S, i, key);
    S = S->forward[1];

    // duplicate
    if (key == S->key) {
        pre_value = S->value;
        S->value = value;
        dupli=1;
        return;
    } else {
        // random number generator
        while (2 * rand() < RAND_MAX) {
            if(level == MAX_LEVEL)break;
            level++;
        }

        if (level > list->level) {
            for (i = list->level; i < level; i++) {
                temp[i+1] = list->header;
            }
            list->level = level;
        }
        S = (SKlist) malloc(sizeof(skiplist));
        S->key = key;
        S->value = value;
        S->forward = (SKlist *)malloc(sizeof(SKlist) * (level + 1));
        for (i = 1; i <= level; i++) {
            S->forward[i] = temp[i]->forward[i];
            temp[i]->forward[i] = S;
        }
    }
}

SKlist Find(Levels list, int key) {
    SKlist S = list->header;
    for (int i = list->level; i >= 1; i--) S = level_down(S,i,key);
    if (S->forward[1]->key == key) {
        return S->forward[1];
    } else {
        notfound = 1;
        return NULL;
    }
}

int main() {
    int i;
    Levels list;
    SKlist F;
    list = (Levels)malloc(sizeof(levelist));
    SKlist H = (SKlist) malloc(sizeof(struct skiplist)); // H is Header

    // header setting
    list->header = H;
    H->key = ULLONG_MAX;
    H->forward = (SKlist *) malloc(sizeof(SKlist) * (MAX_LEVEL + 1));
    for (i = 0; i <= MAX_LEVEL; i++) {
        H->forward[i] = list->header;
    }
    list->level = 1;

    char infi; // insert, find
    uint64_t key,val;
    FILE *open = fopen("input.txt","r");
    close = fopen("output.txt","w");
    if(open==NULL){puts("err : file read"); return 0;}

    clock_gettime(CLOCK_MONOTONIC, &start);

    while(fscanf(open,"%c",&infi) != EOF)
    {
        switch(infi) {
            case 'I':
                fscanf(open, "%lld%lld", &key, &val);
                Insert(list, key, val);
                if(dupli==1){
                    fprintf(close,"Found (%lld,%lld) update v=%lld\n",key,pre_value,val);
                    dupli=0;
                }
                else fprintf(close,"Inserted (%lld,%lld)\n",key,val);
                break;
            case 'F':
                fscanf(open, "%lld", &key);
                F = Find(list, key);
                if(notfound==1){
                    fprintf(close,"Not Found\n");
                    notfound=0;
                }
                else{
                    fprintf(close,"Found (%lld,%lld)\n",F->key,F->value);
                }
                break;
            case 'Q':
                // end of file
                break;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed_time = (end.tv_nsec - start.tv_nsec) + (end.tv_sec - start.tv_sec) * BILLION;
    printf("elapsed_time : %f", elapsed_time);

    // list free
    SKlist current = list->header->forward[1];
    SKlist next;
    while(1) {
        if(current == list->header)break;
        next = current->forward[1];
        free(current->forward);
        free(current);
        current = next;
    }
    free(current->forward);
    free(current);
    free(list);

    return 0;
}