#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/queue.h>

void *long_term_scheduler(void *ptr);
void *short_term_scheduler(void *ptr);
int print_queues(void *ptr, char *s);
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

    while (!TAILQ_EMPTY(&job_head) || !TAILQ_EMPTY(&ready_head)) {
        if (!TAILQ_EMPTY(&job_head)) {
            /* create and invoke long term scheduler thread */
            if ((n = pthread_create(&lts, NULL, &long_term_scheduler, node)) != 0) {
                perror("[Kernel] Error creating long term scheduler thread");
                return n;
            }
            printf("------------------------------------\n");
            printf("[Kernel] Long Term Scheduler Invoked\n");
            printf("------------------------------------\n\n");

            pthread_join(lts, NULL);
        }

        /* create and invoke short term scheduler thread */
        if ((n = pthread_create(&sts, NULL, &short_term_scheduler, node)) != 0) {
            perror("[Kernel] Error creating short term scheduler thread");
            return n;
        }
        printf("-------------------------------------\n");
        printf("[Kernel] Short Term Scheduler Invoked\n");
        printf("-------------------------------------\n\n");

        pthread_join(sts, NULL);
    }

    printf("---------------------------------------------\n");
    printf("[Kernel] All processes successfully finished!\n");
    printf("---------------------------------------------\n");
    return 0;
}

/* function executed by long term scheduler thread */
void *long_term_scheduler(void *ptr) {
    struct process *p = (struct process *) ptr;
    char sched_name[5] = "[LTS]";
    int n;

    if ((n = print_queues(p, sched_name)) != 0)
        perror("[LTS] Error printing job queue and/or ready queue\n\n");

    n = 0;
    while (n < 5 && TAILQ_FIRST(&job_head) != NULL) {
        p = TAILQ_FIRST(&job_head);
        TAILQ_REMOVE(&job_head, p, job_q);
        TAILQ_INSERT_TAIL(&ready_head, p, ready_q);
        printf("[LTS] Process %d removed from the Job Queue and inserted into the Ready Queue\n", p->pid);
        n++;
    }
    printf("[LTS] Ready Queue is Full, cannot enter more\n\n");

    if ((n = print_queues(p, sched_name)) != 0)
        perror("[LTS] Error printing job queue and/or ready queue\n\n");

    return NULL;
}

/* function executed by short term scheduler thread */
void *short_term_scheduler(void *ptr) {
    struct process *p = (struct process *) ptr;
    char sched_name[5] = "[STS]";
    int n;

    if ((n = print_queues(p, sched_name)) != 0)
        perror("[STS] Error printing job queue and/or ready queue\n\n");

    n = 0;
    while (n < 5 && !TAILQ_EMPTY(&ready_head)) {
        p = TAILQ_FIRST(&ready_head);
        TAILQ_REMOVE(&ready_head, p, ready_q);
        printf("[STS] Process %d now executing\n", p->pid);
        p->time -= 2;

        if (p->time <= 0)
            printf("[STS] Process %d terminated\n\n", p->pid);
        else {
            TAILQ_INSERT_TAIL(&ready_head, p, ready_q);
            printf("[STS] Process %d with remaining time %d enqueued to the Ready Queue\n\n", p->pid, p->time);
        }

        n++;
    }

    if ((n = print_queues(p, sched_name)) != 0)
        perror("[STS] Error printing job queue and/or ready queue\n\n");

    return NULL;
}

/* function used by both schedulers to display job and ready queues */
int print_queues(void *ptr, char *s) {
    struct process *p = (struct process *) ptr;
    
    if (p == NULL || s == NULL)
        return -1;

    printf("%s", s);
    printf(" Job Queue: ");
    if (TAILQ_EMPTY(&job_head))
        printf("EMPTY");
    else {
        TAILQ_FOREACH(p, &job_head, job_q) {

            printf("[Process %d: Time %d]", p->pid, p->time);
            if (p != TAILQ_LAST(&job_head, j_head))
                printf(", ");

        }
    }
    printf("\n\n");

    printf("%s", s);
    printf(" Ready Queue: ");
    if (TAILQ_EMPTY(&ready_head))
        printf("EMPTY");
    else {

        TAILQ_FOREACH(p, &ready_head, ready_q) {
            printf("[Process %d: Time %d]", p->pid, p->time);
            if (p != TAILQ_LAST(&ready_head, r_head))
                printf(", ");
        }

    }
    printf("\n\n");

    return 0;
}