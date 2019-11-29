#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/queue.h>

void *long_term_scheduler(void *process);
void *short_term_scheduler(void *process);
TAILQ_HEAD(r_head, process) job_head;
TAILQ_HEAD(j_head, process) ready_head;

struct process {
    /* unique ID of process */
    int pid;
    /* total execution time of process */
    int time;
    /* pointer to job queue */
    TAILQ_ENTRY(process) job_q;
    /* pointer to ready queue */
    TAILQ_ENTRY(process) ready_q;
};

int main() {
    /* threads representing long term scheduler and short term scheduler */
    pthread_t lts, sts;
    TAILQ_INIT(&job_head);
    TAILQ_INIT(&ready_head);

    int n;
    struct process *node;

    /* populate job queue */
    for (n = 0; n < 100; n++) {
        node = NULL;
        node = malloc(sizeof(struct process));
        node->pid = n + 1;
        node->time = rand() % 30 + 1;
        printf("[Kernel] Process %d created with time = %d\n", node->pid, node->time);
        TAILQ_INSERT_TAIL(&job_head, node, job_q);
    }
    printf("\n");

    /* create long term scheduler thread */
    if ((n = pthread_create(&lts, NULL, &long_term_scheduler, node)) != 0) {
        perror("Error creating long term scheduler thread.");
        return n;
    }
    printf("[Kernel] Long Term Scheduler Invoked\n");
    printf("\n");

    pthread_join(lts, NULL);
    printf("\n");

    /* create short term scheduler thread */
    if ((n = pthread_create(&sts, NULL, &short_term_scheduler, node)) != 0) {
        perror("Error creating short term scheduler thread.");
        return n;
    }
    printf("[Kernel] Short Term Scheduler Invoked\n");
    printf("\n");

    pthread_join(sts, NULL);
    return 0;
}

/* function executed by long term scheduler thread */
void *long_term_scheduler(void *ptr) {
    struct process *p = (struct process *) ptr;
    int n;

    printf("[LTS] Job Queue: ");
    TAILQ_FOREACH(p, &job_head, job_q) {

        printf("[Process %d: Time %d]", p->pid, p->time);
        if (p != TAILQ_LAST(&job_head, j_head))
            printf(", ");

    }
    printf("\n\n");

    printf("[LTS] Ready Queue: ");
    if (TAILQ_EMPTY(&ready_head))
        printf("EMPTY");
    else {

        TAILQ_FOREACH(p, &ready_head, ready_q) {
            printf("[Process %d: Time %d]", p->pid, p->time);
            if (p != TAILQ_LAST(&ready_head, r_head))
                printf(", ");
        }

    }
    
    printf("\n");

    return NULL;
}

/* function executed by short term scheduler thread */
void *short_term_scheduler(void *ptr) {
    struct process *p = (struct process *) ptr;

    return NULL;
}