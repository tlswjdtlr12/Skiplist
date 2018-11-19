#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define SKIPLIST_MAX_LEVEL 10

typedef struct skiplist * SKlist;
typedef struct levelist * Levels;

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

int skiplist_insert(Levels list, uint64_t key, uint64_t value) {
    SKlist update[SKIPLIST_MAX_LEVEL + 1];
    SKlist x = list->header;
    int i, level;
    for (i = list->level; i >= 1; i--) {
        while (x->forward[i]->key < key)
            x = x->forward[i];
        update[i] = x;
    }
    x = x->forward[1];

    if (key == x->key) {
        x->value = value;
        return 0;
    } else {
        level = rand_level();
        if (level > list->level) {
            for (i = list->level + 1; i <= level; i++) {
                update[i] = list->header;
            }
            list->level = level;
        }

        x = (SKlist) malloc(sizeof(skiplist));
        x->key = key;
        x->value = value;
        x->forward = (SKlist * ) malloc(sizeof(SKlist) * (level + 1));
        for (i = 1; i <= level; i++) {
            x->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = x;
        }
    }
    return 0;
}

SKlist skiplist_search(Levels list, int key) {
    SKlist x = list->header;
    int i;
    for (i = list->level; i >= 1; i--) {
        while (x->forward[i]->key < key)
            x = x->forward[i];
    }
    if (x->forward[1]->key == key) {
        return x->forward[1];
    } else {
        return NULL;
    }
}

static void skiplist_node_free(SKlist x) {
    if (x) {
        free(x->forward);
        free(x);
    }
}

int skiplist_delete(Levels list, int key) {
    int i;
    SKlist update[SKIPLIST_MAX_LEVEL + 1];
    SKlist x = list->header;
    for (i = list->level; i >= 1; i--) {
        while (x->forward[i]->key < key)
            x = x->forward[i];
        update[i] = x;
    }

    x = x->forward[1];
    if (x->key == key) {
        for (i = 1; i <= list->level; i++) {
            if (update[i]->forward[i] != x)
                break;
            update[i]->forward[i] = x->forward[i];
        }
        skiplist_node_free(x);

        while (list->level > 1 && list->header->forward[list->level]
                                  == list->header)
            list->level--;
        return 0;
    }
    return 1;
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

//static void skiplist_dump(Levels list) {
//    SKlist x = list->header;
//    while (x && x->forward[1] != list->header) {
//        printf("%d[%d]->", x->forward[1]->key, x->forward[1]->value);
//        x = x->forward[1];
//    }
//    printf("NIL\n");
//}

int main() {
    int arr[] = { 3, 6, 9, 2, 11, 1, 4 }, i;
    Levels list;
    list = (Levels)malloc(sizeof(levelist));

    SKlist header = (SKlist) malloc(sizeof(struct skiplist));
    list->header = header;

    // 조낸 큰값이 필요한 알고리즘인듯
    header->key = 100000;

    header->forward = (SKlist *) malloc(sizeof(SKlist) * (SKIPLIST_MAX_LEVEL + 1));
    for (i = 0; i <= SKIPLIST_MAX_LEVEL; i++) {
        header->forward[i] = list->header;
    }
    list->level = 1;

    printf("Insert:--------------------\n");
    for (i = 0; i < sizeof(arr) / sizeof(arr[0]); i++) {
        skiplist_insert(list, arr[i], arr[i]);
    }
   // skiplist_dump(list);

    printf("Search:--------------------\n");
    int keys[] = { 3, 4, 7, 10, 111 };

    for (i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
        SKlist x = skiplist_search(list, keys[i]);
        if (x) {
            printf("key = %d, value = %d\n", keys[i], x->value);
        } else {
            printf("key = %d, not fuound\n", keys[i]);
        }
    }

    printf("Search:--------------------\n");
    skiplist_delete(list, 3);
    skiplist_delete(list, 9);
  //  skiplist_dump(list);
    skiplist_free(list);

    return 0;
}
//
//int main(int argc, char * argv[]) {
//    RBTree T, F;
//
//    // NN is node with null
//    if (NN == NULL) {
//        NN = malloc(sizeof ( struct RedBlackNode));
//        if (NN == NULL) printf("error");
//        NN->Left = NN->Right = NN;
//        NN->Color = Black;
//    }
//
//    // T is header node. T->Right is root
//    T = malloc(sizeof(struct RedBlackNode));
//    if (T == NULL) printf("error\n");
//    T->Left = T->Right = NN;
//    T->Color = Black;
//
//    printf("Inserts are complete\n");
//    //PrintTree(T->Right); // header skip
//
//    char infi; // insert, find
//    int key,val;
//    FILE *open = fopen("input.txt","r");
//    close = fopen("output.txt","w");
//    if(open==NULL){puts("err : file read"); return 0;}
//
//    while(fscanf(open,"%c",&infi) != EOF)
//    {
//        switch(infi) {
//            case 'I':
//                fscanf(open, "%d%d", &key, &val);
//                T = Insert(key, val, T);
//                if(dupli==1){
//                    fprintf(close,"Found (%d,%d) update v=%d\n",key,pre_value,val);
//                    dupli=0;
//                }
//                else fprintf(close,"Inserted (%d,%d)\n",key,val);
//                break;
//            case 'F':
//                fscanf(open, "%d", &key);
//                F = Find(key,T);
//                if(notfound==1){
//                    fprintf(close,"Not Found\n");
//                    notfound=0;
//                    depth=0;
//                }
//                else{
//                    fprintf(close,"Found (%d,%d) on d=%d with c=",F->Key,F->Value,depth-1); // depth-1 : because header
//                    if(F->Color==Red)fprintf(close,"red\n");
//                    else fprintf(close,"black\n");
//                    depth=0;
//                }
//                break;
//            case 'P':
//                PrintTree(T->Right, close);
//                fprintf(close, "\n");
//            case 'Q':
//                // end of file
//                break;
//        }
//    }
//    return 0;
//}