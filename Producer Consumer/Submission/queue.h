#pragma once

#include <stdio.h>
#include <stdlib.h>

#define QUEUE_SIZE 10

typedef struct process {
    int producer_pid;           // Producer process_id
    int producer_no;            // Producer Number
    int priority;               // Priority between 1 and 10
    int compute_time;           // Compute Time between 1 and 4
    int job_id;                 // Job ID between 1 and 100000
} PROC;

struct priority_queue {
    PROC arr[QUEUE_SIZE];       // Queue array
    int back;                   // Last index of Queue;
};

void init_queue(struct priority_queue *pq);
int is_empty(struct priority_queue pq);
int is_full(struct priority_queue pq);
int enqueue(struct priority_queue *pq, PROC proc);
PROC dequeue(struct priority_queue *pq);
void print_priority_queue(struct priority_queue pq);