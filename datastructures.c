#include <string.h>
#include <stdlib.h>

#include "globals.h"
#include "utils.h"
#include "datastructures.h"
// #include "shell_manipulation.h"

void init_queue(struct queue **Q) {
    *Q = malloc(sizeof(struct queue**));

    (*Q)->rear = malloc(sizeof(struct string_node));
    (*Q)->rear->string =  malloc(MAX_COMMAND_LEN * sizeof(char));
    (*Q)->rear->next = NULL;
    (*Q)->front = (*Q)->rear;
    (*Q)->size = 0;
}
