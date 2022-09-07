// store history in file (persistent storage)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../globals.h"
#include "../shell_manipulation.h"

void add_command_to_history(struct ShellVariables *sv, char *command) {
    /*
    Cases:
    Search for the command in the queue
    If it is not present, add it at the rear
    If it is not present, but the queue is already full, change the front 
    pointer to "delete" the oldest element, and then add the new element to the rear

    If it is present, then delete it from the queue. Shift remaining elements accordingly, 
    and then do as mentioned above to add it to the queue
    */
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

    int size = 0;
    int front = sv->command_queue_front;
    int rear  = sv->command_queue_rear;

    if(front > rear) {
        size = (MAX_COMMANDS_IN_HISTORY - front) + (rear+1);
    } else {
        size = rear - front + 1;
    }

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

    if(num_commands > size) {
        num_commands = size;
    }

    int i = 0;
    
    // printf("F: %d\nR: %d\n", front, rear);
    // printf("size: %d\n", size);
    // printf("num comm: %d\n", num_commands);
    // printf("aa  %d\n", j);

    int j = MAX_COMMANDS_IN_HISTORY-1;

    while(i < num_commands) {
        if(front <= rear) {
            printf("%d  %s\n", i+1, sv->command_queue[rear-i]);
        } else {
            if(rear-i >= 0) {
                int index = rear - i;
                printf("%d  %s\n", i+1, sv->command_queue[index]);
            } else {
                int index = j;
                printf("%d  %s\n",  i+1, sv->command_queue[index]);
                j--;
            }
        }
        i++;
    }



}
