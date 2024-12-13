# CPSC_351_Proj_3

Timothy Hulse (joubu.tim@csu.fullerton.edu)



The language is in C

How It Works

Initialization:
Reads memory size, page size, and process details from user input and a file.
Sets up a process queue and divides memory into frames.
Simulation:
Runs in a loop, simulating time increments.
Processes are:
Enqueued upon arrival.
Allocated memory if available.
Deallocated upon completion.
The loop stops when all processes finish or time exceeds the limit.
Output:
Tracks process states and memory allocation.
Displays the average turnaround time of all processes.
Key Files

Input File: Contains process details (arrival time, execution time, and memory requirements).
Source Code: Implements the simulation and memory management logic.

Here is an Example of the code running.

![image](https://github.com/user-attachments/assets/99d9aad6-6968-4dbd-96be-1b201bf0ee6c)
