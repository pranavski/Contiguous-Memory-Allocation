#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 100

typedef struct {
    int pid;    
    int size;    
    int isFree;  
} MemoryBlock;

MemoryBlock memory[MAX_PROCESSES];
int numBlocks = 1;  
int memorySize;     


void initMemory(int size);
void requestMemory(int pid, int size, char strategy);
void releaseMemory(int pid);
void compactMemory();
void printMemoryStatus();

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <memory_size>\n", argv[0]);
        return 1;
    }

    memorySize = atoi(argv[1]);
    initMemory(memorySize);

    char command[10];
    while (1) {
        printf("Allocate>>");
        scanf("%s", command);

        if (strcmp(command, "RQ") == 0) {
            int pid, size;
            char strategy;
            scanf("%d %d %c", &pid, &size, &strategy);
            requestMemory(pid, size, strategy);
        } else if (strcmp(command, "RL") == 0) {
            int pid;
            scanf("%d", &pid);
            releaseMemory(pid);
        } else if (strcmp(command, "C") == 0) {
            compactMemory();
        } else if (strcmp(command, "STAT") == 0) {
            printMemoryStatus();
        } else if (strcmp(command, "X") == 0) {
            break;
        } else {
            printf("Invalid command\n");
        }
    }

    return 0;
}

// initMemory: Initializes the memory with a single free block of the given size
void initMemory(int size) {
    memory[0].pid = 0;
    memory[0].size = size;
    memory[0].isFree = 1;
}

// requestMemory: Allocates memory for a process based on the specified strategy 
void requestMemory(int pid, int size, char strategy) {
    int bestIndex = -1;
    int bestSize = memorySize + 1;

    for (int i = 0; i < numBlocks; i++) {
        if (memory[i].isFree && memory[i].size >= size) {
            if (strategy == 'F') {
                bestIndex = i;
                break;
            } else if (strategy == 'B') {
                if (memory[i].size < bestSize) {
                    bestIndex = i;
                    bestSize = memory[i].size;
                }
            } else if (strategy == 'W') {
                if (memory[i].size > bestSize) {
                    bestIndex = i;
                    bestSize = memory[i].size;
                }
            }
        }
    }

    if (bestIndex == -1) {
        printf("Cannot allocate memory for process %d\n", pid);
        return;
    }

    if (memory[bestIndex].size > size) {
        memory[numBlocks].pid = 0;
        memory[numBlocks].size = memory[bestIndex].size - size;
        memory[numBlocks].isFree = 1;
        numBlocks++;

        memory[bestIndex].size = size;
    }

    memory[bestIndex].pid = pid;
    memory[bestIndex].isFree = 0;
    printf("Allocated %d bytes for process %d\n", size, pid);
}

// releaseMemory: Frees the memory allocated to a process and merges adjacent free blocks
void releaseMemory(int pid) {
    int index = -1;
    for (int i = 0; i < numBlocks; i++) {
        if (memory[i].pid == pid) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("Process %d is not allocated\n", pid);
        return;
    }

    memory[index].pid = 0;
    memory[index].isFree = 1;

    for (int i = 0; i < numBlocks - 1; i++) {
        if (memory[i].isFree && memory[i + 1].isFree) {
            memory[i].size += memory[i + 1].size;
            for (int j = i + 1; j < numBlocks - 1; j++) {
                memory[j] = memory[j + 1];
            }
            numBlocks--;
            i--;
        }
    }

    printf("Released memory for process %d\n", pid);
}

// compactMemory: Compacts the memory by moving all allocated blocks to the beginning and merging all free blocks
void compactMemory() {
    int freeIndex = 0;
    int allocatedIndex = 1;

    for (int i = 0; i < numBlocks; i++) {
        if (memory[i].isFree) {
            memory[freeIndex] = memory[i];
            freeIndex++;
        } else {
            memory[allocatedIndex] = memory[i];
            allocatedIndex++;
        }
    }

    memory[0].pid = 0;
    memory[0].isFree = 1;
    memory[0].size = 0;

    for (int i = 0; i < freeIndex; i++) {
        memory[0].size += memory[i].size;
    }

    numBlocks = freeIndex + allocatedIndex - 1;
    printf("Memory compacted\n");
}

// printMemoryStatus: Prints the current status of the memory, showing the start address, size, and process ID for each block
void printMemoryStatus() {
    printf("Memory Status:\n");
    printf("%-10s %-10s %-10s\n", "Start", "Size", "Process");
    int start = 0;
    for (int i = 0; i < numBlocks; i++) {
        if (memory[i].isFree) {
            printf("%-10d %-10d FREE\n", start, memory[i].size);
        } else {
            printf("%-10d %-10d %-10d\n", start, memory[i].size, memory[i].pid);
        }
        start += memory[i].size;
    }
}