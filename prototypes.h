#include "process.h"
#include "queue.h"
#include "memory.h"

PROCESS* assign_process_list(const char* file_path);
char* get_announcement_prefix(int current_time);
int get_number_of_processes_from_file(FILE* filePtr);
int main(void);
int process_numeric_input_from_user(const char* output, int (*func)(int));
void assign_available_memory_to_waiting_procs(int current_time);
void clear_stdin(char* buf);
void terminate_completed_procs(int current_time);
void enqueue_newly_arrived_procs(int current_time);
void get_user_input(int* mem, int* page, char* file_path);
int is_one_two_or_three(int t);
void main_loop(void);
int multiple_of_one_hundred(int t);
void print_turnaround_times(void);
void prompt_for_filename(char* res);

