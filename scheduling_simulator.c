#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 100

// Structure to store process details
typedef struct {
    int pid, arrival, burst, priority;
    int waiting_time, turnaround_time, remaining_time, start_time;
} Process;

// Function to read processes from a file
int readProcesses(const char *filename, Process processes[]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Unable to open file.\n");
        return -1;
    }

    char header[50];
    fgets(header, sizeof(header), file);  // Skip the header line

    int i = 0;
    while (fscanf(file, "%d %d %d %d", &processes[i].pid, &processes[i].arrival, 
                  &processes[i].burst, &processes[i].priority) == 4) {
        processes[i].remaining_time = processes[i].burst;
        i++;
    }
    fclose(file);
    return i;  // Return number of processes
}

// Function to implement First-Come, First-Served (FCFS) scheduling
void FCFS(Process processes[], int n) {
    int current_time = 0;
    float total_wt = 0, total_tat = 0;
    
    printf("\nGantt Chart (FCFS):\n");
    for (int i = 0; i < n; i++) {
        if (current_time < processes[i].arrival)
            current_time = processes[i].arrival;

        processes[i].start_time = current_time;
        processes[i].waiting_time = processes[i].start_time - processes[i].arrival;
        processes[i].turnaround_time = processes[i].waiting_time + processes[i].burst;
        current_time += processes[i].burst;
        
        total_wt += processes[i].waiting_time;
        total_tat += processes[i].turnaround_time;
        
        printf("| P%d ", processes[i].pid);
    }
    printf("|\n");

    printf("\nPID\tArrival\tBurst\tWT\tTAT\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\n", processes[i].pid, processes[i].arrival, 
               processes[i].burst, processes[i].waiting_time, processes[i].turnaround_time);
    }
    printf("Avg WT: %.2f, Avg TAT: %.2f\n", total_wt / n, total_tat / n);
}

// Function to implement Round Robin (RR) Scheduling
void RoundRobin(Process processes[], int n, int time_quantum) {
    int current_time = 0, completed = 0;
    float total_wt = 0, total_tat = 0;

    printf("\nGantt Chart (Round Robin, Quantum = %d):\n", time_quantum);
    
    while (completed < n) {
        for (int i = 0; i < n; i++) {
            if (processes[i].remaining_time > 0 && processes[i].arrival <= current_time) {
                printf("| P%d ", processes[i].pid);
                int exec_time = (processes[i].remaining_time > time_quantum) ? time_quantum : processes[i].remaining_time;
                current_time += exec_time;
                processes[i].remaining_time -= exec_time;

                if (processes[i].remaining_time == 0) {
                    processes[i].turnaround_time = current_time - processes[i].arrival;
                    processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst;
                    total_wt += processes[i].waiting_time;
                    total_tat += processes[i].turnaround_time;
                    completed++;
                }
            }
        }
    }
    printf("|\n");

    printf("\nPID\tArrival\tBurst\tWT\tTAT\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\n", processes[i].pid, processes[i].arrival, 
               processes[i].burst, processes[i].waiting_time, processes[i].turnaround_time);
    }
    printf("Avg WT: %.2f, Avg TAT: %.2f\n", total_wt / n, total_tat / n);
}

int main() {
    char filename[] = "processes.txt";
    Process processes[MAX_PROCESSES];

    int n = readProcesses(filename, processes);
    if (n <= 0) return 1;

    int choice;
    printf("Choose Scheduling Algorithm:\n1. FCFS\n2. Round Robin\n");
    scanf("%d", &choice);

    if (choice == 1) {
        FCFS(processes, n);
    } else if (choice == 2) {
        int time_quantum;
        printf("Enter Time Quantum: ");
        scanf("%d", &time_quantum);
        RoundRobin(processes, n, time_quantum);
    } else {
        printf("Invalid choice.\n");
    }

    return 0;
}
