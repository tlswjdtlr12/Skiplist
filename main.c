#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define SKIPLIST_MAX_LEVEL 10

typedef struct skiplist * SKlist;
typedef struct levelist * Levels;
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

static int rand_level() {
    int level = 1;
    while (rand() < RAND_MAX / 2 && level < SKIPLIST_MAX_LEVEL)
        level++;
    return level;
}

void Insert(Levels list, uint64_t key, uint64_t value) {
    SKlist update[SKIPLIST_MAX_LEVEL + 1];
    SKlist S = list->header;
    int i, level;
    for (i = list->level; i >= 1; i--) {
        while (S->forward[i]->key < key)
            S = S->forward[i];
        update[i] = S;
    }
    S = S->forward[1];

    if (key == S->key) {
        pre_value = S->value;
        S->value = value;
        dupli=1;
        return;
    } else {
        level = rand_level();
        if (level > list->level) {
            for (i = list->level + 1; i <= level; i++) {
                update[i] = list->header;
            }
            list->level = level;
        }

        S = (SKlist) malloc(sizeof(skiplist));
        S->key = key;
        S->value = value;
        S->forward = (SKlist * ) malloc(sizeof(SKlist) * (level + 1));
        for (i = 1; i <= level; i++) {
            S->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = S;
        }
    }
}

SKlist Find(Levels list, int key) {
    SKlist S = list->header;
    int i;
    for (i = list->level; i >= 1; i--) {
        while (S->forward[i]->key < key)
            S = S->forward[i];
    }
    if (S->forward[1]->key == key) {
        return S->forward[1];
    } else {
        notfound = 1;
        return NULL;
    }
}

void skiplist_free(Levels list)
{
    SKlist current_node = list->header->forward[1];
    while(current_node != list->header) {
        SKlist next_node = current_node->forward[1];
        free(current_node->forward);
        free(current_node);
        current_node = next_node;
    }
    free(current_node->forward);
    free(current_node);
    free(list);
}
// 출력법
static void skiplist_dump(Levels list) {
    SKlist S = list->header;
    while (S && S->forward[1] != list->header) {
        printf("%d[%d]->", S->forward[1]->key, S->forward[1]->value);
        S = S->forward[1];
    }
    printf("NIL\n");
}

int main() {
    int i;
    Levels list;
    SKlist F;
    list = (Levels)malloc(sizeof(levelist));
    SKlist H = (SKlist) malloc(sizeof(struct skiplist)); // H is Header

    list->header = H;
    H->key = ULLONG_MAX; // 조낸 큰값이 필요한 알고리즘인듯
    H->forward = (SKlist *) malloc(sizeof(SKlist) * (SKIPLIST_MAX_LEVEL + 1));
    for (i = 0; i <= SKIPLIST_MAX_LEVEL; i++) {
        H->forward[i] = list->header;
    }
    list->level = 1;

    char infi; // insert, find
    uint64_t key,val;
    FILE *open = fopen("input.txt","r");
    close = fopen("output.txt","w");
    if(open==NULL){puts("err : file read"); return 0;}

    while(fscanf(open,"%c",&infi) != EOF)
    {
        switch(infi) {
            case 'I':
                fscanf(open, "%lld%lld", &key, &val);
                Insert(list, key, val);
                //skiplist_dump(list);
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

  //  skiplist_dump(list);
    skiplist_free(list);

    return 0;
}