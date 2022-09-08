// store history in file (persistent storage)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../globals.h"
#include "../utils.h"
#include "../datastructures.h"
#include "../shell_manipulation.h"

void update_history_db(struct ShellVariables *sv) {
    struct queue **Q = sv->command_buffer;

    remove("history_database.txt");
    FILE *history_db = fopen("history_database.txt", "w");
    struct string_node *current = (*sv->command_buffer)->front;
    
    while(current->next != NULL) {
        char line[MAX_COMMAND_LEN];
        sprintf(line, "%s\n", current->string);
        fputs(line, history_db);
        current = current->next;
    }

    fclose(history_db);
}

void add_command_to_history(struct ShellVariables *sv, const char *command) {
    /*
    We're maintaining a 'moving' queue with nodes
    Push the command at the rear, if it is not the last command
    If the size exceeds the buffer limit, then update the front of the queue and free the deleted element
    */

    struct queue **Q = sv->command_buffer;


    if((*Q)->size  == 0) {
        strcpy((*Q)->rear->string, command);
        (*Q)->size++;
        return;
    }

    if(strcmp((*Q)->rear->string, command) == 0) {
        return;
    }

    struct string_node *element = malloc(sizeof(struct string_node));
    element->string = malloc(MAX_COMMAND_LEN * sizeof(char));
    strcpy(element->string, command);
    element->next = NULL;
    
    (*Q)->rear->next = element;
    (*Q)->rear = element;

    if((*Q)->size < MAX_COMMANDS_IN_HISTORY) {
        (*Q)->size++;
    } else {
        struct string_node *element = (*Q)->front;
        (*Q)->front = (*Q)->front->next;
        free(element->string);
        free(element);
    }
}

void fetch_history_db(struct ShellVariables *sv) {

    FILE *history_db = fopen("history_database.txt", "r");

    if(history_db == NULL) {
        return;
    }

    char line[MAX_COMMAND_LEN];

    while(fscanf(history_db, "%[^\n]%*c", line)!= EOF) {
        add_command_to_history(sv, line);
    }
    
    fclose(history_db);
}

void run_history(const struct ShellVariables *sv) {
    /*
    Commands run are always stored in the shell variables
    Using a custom circular queue, to make things easier
    Add elements to the queue (at the rear), if not already present
    If the element is already present, remove it from the queue, and push it at the end

    While printing, we want to print in reverse chronological order, so print the
    queue from the rear to the front
    */
    
    char *arg = strtok(NULL, " ");

    int num_commands = 10;
    if(arg != NULL) {
        char num_string[MAX_PATH_LEN];
        strcpy(num_string, arg);

        num_commands = atoi(num_string);

        if(num_commands > 20) {
            num_commands = 20;
        }

        arg = strtok(NULL, " ");
        if(arg != NULL) {
            shell_warning("history takes only one integer argument");
            return;
        }
    }

    // accessing sv this way, because we're modifying a struct, contained in a struct (modification of pointer to pointer)
    if(num_commands > (*sv->command_buffer)->size) {
        num_commands = (*sv->command_buffer)->size;
    }

    int i = 0;
    struct string_node *current = (*sv->command_buffer)->front;
    while(i < (*sv->command_buffer)->size - num_commands) {
        current = current->next;
        i++;
    }

    int count = 1;
    while(current->next != NULL) {
        printf("%d  %s\n", count++, current->string);
        current = current->next;
    }

    printf("%d  %s\n", count, current->string);
}
