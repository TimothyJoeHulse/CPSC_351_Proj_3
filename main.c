#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "prototypes.h"

// global constants
const int TIME_MAX = 100000;

// shared data
int number_of_procs = 0, last_announcement = -1;
PROCESS* proc_list;
proc_queue* queue;
frame_list* framelist;

void main_loop(void) {
    long current_time = 0;

    while (1) {
        // queue any procs that have arrived
        enqueue_newly_arrived_procs(current_time);

        // remove any completed procs
        terminate_completed_procs(current_time);

        // assign available memory to procs that need it
        assign_available_memory_to_waiting_procs(current_time);

        current_time++;

        if (current_time > TIME_MAX) {
            printf("DEADLOCK: max time reached\n");
            break;
        }

        if (queue->size == 0 && frame_list_is_empty(framelist)) {
            break;
        }
    }

    print_turnaround_times();
}

int main(void) {
    int page_size  = 0;
    int mem_size   = 0;

    char* file_path = malloc(100 * sizeof(char));

    /*******************************************************************
     * FIRST PHASE
     ******************************************************************/

    // get the user input
    get_user_input(&mem_size, &page_size, file_path);

    // read values from the input'd file into a shared proc list
    proc_list = assign_process_list(file_path);

    // create a shared queue with a capacity = # of procs
    queue = create_proc_queue(number_of_procs);

    // create a shared framelist
    framelist = create_frame_list(mem_size / page_size, page_size);

    main_loop();

    return 0;
}

void enqueue_newly_arrived_procs(int current_time) {
    int i;
    PROCESS* proc;

    for (i = 0; i < number_of_procs; i += 1) {
        proc = &proc_list[i];

        if (proc->arrival_time == current_time) {
            printf("%sProcess %d arrives\n",
                   get_announcement_prefix(current_time),
                   proc->pid);

            enqueue_proc(queue, proc);

            print_proc_queue(queue);
            print_frame_list(framelist);
        }
    }
}

void terminate_completed_procs(int current_time) {
    int i, time_spent_in_memory;
    PROCESS* proc;

    // dequeue any procs that need it
    for (i = 0; i < number_of_procs; i += 1) {
        proc = &proc_list[i];
        time_spent_in_memory = current_time - proc->time_added_to_memory;

        if (proc->is_active && (time_spent_in_memory >= proc->life_time)) {
            printf("%sProcess %d completes\n",
                   get_announcement_prefix(current_time),
                   proc->pid);

            proc->is_active = 0;
            proc->time_finished = current_time;

            free_memory_for_pid(framelist, proc->pid);

            print_frame_list(framelist);
        }
    }
}

void assign_available_memory_to_waiting_procs(int current_time) {
    int i, index, limit;
    PROCESS* proc;

    limit = queue->size;

    // enqueue any procs that can be put into mem
    for (i = 0; i < limit; i += 1) {
        index = iterate_queue_index(queue, i);
        proc = queue->elements[index];

        if (proc_can_fit_into_memory(framelist, proc)) {
            printf("%sMM moves Process %d to memory\n",
                   get_announcement_prefix(current_time),
                   proc->pid);

            fit_proc_into_memory(framelist, proc);

            proc->is_active = 1;
            proc->time_added_to_memory = current_time;

            dequeue_proc_at_index(queue, i);
            print_proc_queue(queue);
            print_frame_list(framelist);
        }
    }
}

char* get_announcement_prefix(int current_time) {
    char* result;

    result = malloc(20 * sizeof(char));

    if (last_announcement == current_time) {
        sprintf(result, "\t");
    } else {
        sprintf(result, "t = %d: ", current_time);
    }

    last_announcement = current_time;

    return result;
}

void print_turnaround_times(void) {
    int i;
    float total = 0;

    for (i = 0; i < number_of_procs; i += 1) {
        total += proc_list[i].time_finished - proc_list[i].arrival_time;
    }

    printf("Average Turnaround Time: %2.2f\n", total / number_of_procs);
}

int multiple_of_one_hundred(int t) {
    return (t % 100) == 0 ? 1 : 0;
}

int is_one_two_or_three(int t) {
    return (t >= 1 && t <= 3) ? 1 : 0;
}

void clear_stdin(char* buf) {
    if (buf[strlen(buf) - 1] != '\n') {
        int ch;
        while (((ch = getchar()) != '\n') && (ch != EOF)) ;
    }
}

int process_numeric_input_from_user(const char* output, int (*func)(int)) {
    char buf[10];
    int success = 0;
    int res = 0;

    while (!success) {
        printf("%s: ", output);

        if (fgets(buf, 10, stdin) == NULL) {
            clear_stdin(buf);
            printf("ERROR: You didn't enter any data!\n");

            continue;
        }

        if (sscanf(buf, "%d", &res) <= 0) {
            clear_stdin(buf);
            printf("ERROR: You didn't enter a number!\n");

            continue;
        }

        if (!(success = (*func)(res))) {
            clear_stdin(buf);
            printf("ERROR: That number is not a valid choice\n");
        }
    }

    return res;
}

void prompt_for_filename(char* res) {
    char buf[100];
    FILE* fp;

    while (1) {
        printf("Input file: ");

        if (fgets(buf, 100, stdin) == NULL) {
            clear_stdin(buf);
            printf("ERROR: You didn't enter any data!\n");

            continue;
        }

        if (sscanf(buf, "%s", res) <= 0) {
            clear_stdin(buf);
            printf("ERROR: You didn't enter a string!\n");

            continue;
        }

        if (!(fp = fopen(res, "r"))) {
            perror("ERROR: Could not open file!\n");
        } else {
            break;
        }
    }
}

// prompts for memory size and page size
void get_user_input(int* mem, int* page, char* file_path) {
    while (1) {
        *mem = process_numeric_input_from_user(
            "Memory size", multiple_of_one_hundred);

        *page = process_numeric_input_from_user(
            "Page size (1: 100, 2: 200, 3: 400)", is_one_two_or_three);

        switch (*page) {
        case 1: *page = 100; break;
        case 2: *page = 200; break;
        case 3: *page = 400; break;
        }

        if ((*mem) % (*page) == 0) {
            break;
        }

        printf("ERROR: Memory size must be a multiple of the page!");
        printf(" %d is not a multiple of %d, please retry.\n", *mem, *page);
    }

    prompt_for_filename(file_path);
}

// get number of processes from file
int get_number_of_processes_from_file(FILE* filePtr) {
    int num = 0;

    fscanf(filePtr, "%d", &num);

    return num;
}

// stores values processes in process array
PROCESS* assign_process_list(const char* file_path) {
    int numSpace;
    int tmp;
    int counter = 0;
    int totalSpace = 0;
    FILE* filePtr = fopen(file_path, "r");

    number_of_procs = get_number_of_processes_from_file(filePtr);

    // allocate space for process array
    PROCESS* procList = malloc(number_of_procs * sizeof(PROCESS));

    if (!filePtr) {
        printf("ERROR: Failed to open file %s", file_path);
        exit(1);
    }

    while (!feof(filePtr) && counter < number_of_procs) {
        // store values for processes
        fscanf(filePtr, "%d %d %d %d",
               &(procList[counter].pid),
               &(procList[counter].arrival_time),
               &(procList[counter].life_time),
               &numSpace);

        // get total memory requirements for process
        totalSpace = 0;
        for (int i = 0; i < numSpace; i++) {
            fscanf(filePtr, "%d", &tmp);
            totalSpace += tmp;
        }
        procList[counter].mem_reqs = totalSpace;

        procList[counter].is_active = 0;
        procList[counter].time_added_to_memory = -1;
        procList[counter].time_finished = -1;

        counter++;
    }

    fclose(filePtr);

    return procList;
}

