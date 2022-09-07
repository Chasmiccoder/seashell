struct string_node {
    char *string;
    struct string_node *next;
};

struct queue {
    struct string_node *front;
    struct string_node *rear;
    int size;
};

void init_queue(struct queue **Q);