#include "queue.h"

void init_queue(struct priority_queue *pq) {
    pq->back = 0;
}

int is_empty(struct priority_queue pq) {
    if(pq.back==0) return 1;
    return 0;
}

int is_full(struct priority_queue pq) {
    if(pq.back == QUEUE_SIZE) return 1;
    return 0;
}

int enqueue(struct priority_queue *pq, PROC proc) {
    if(QUEUE_SIZE == 0) return -1;
    if(is_full(*pq)) return -2;

    int i=0;
    while(i < pq->back && (pq->arr[i]).priority < proc.priority) i++;
    
    for(int j=pq->back-1; j>=i; j--) 
        pq->arr[j+1] = pq->arr[j];
        
    pq->arr[i] = proc;
    (pq->back)++;
    return 0;
}

PROC dequeue(struct priority_queue *pq) {
    if(is_empty(*pq)) {
        PROC proc;
        proc.job_id = -1;
        return proc;
    }

    (pq->back)--;
    return 
    pq->arr[pq->back];
}

void print_priority_queue(struct priority_queue pq) {
    int i=0;
    printf("\n");
    printf("pq ");
    while(i < pq.back) {
        printf("->(%d, %d, %d, %d)", pq.arr[i].producer_no, pq.arr[i].job_id, pq.arr[i].priority, pq.arr[i].compute_time);
        i++;
    }
    printf("\n");
    printf("\n");
}