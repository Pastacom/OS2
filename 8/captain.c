#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <wait.h>

#define ISLAND_AREAS 15

// Island structure to store in shared memory
typedef struct {
    int areas[ISLAND_AREAS];
    int treasure;
} island;

int pirates;
pid_t captain;
pid_t processes[1024];
island *shm;
int shmid;
int semid;

void cleanup() {
    // Wait for other processes
    while(wait(NULL) > 0);
    printf("Terminating captain process.\n");
    printf("Clearing shared memory and semaphore.\n");
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    exit(0);
}

void sigint_handler() {
    cleanup();
}

void stop() {
    for (int i = 1; i <= pirates; ++i) {
        printf("Pirate %d returned to shore.\n", i);
        kill(processes[i-1], SIGKILL);
    }
    cleanup();
}

int main(int argc, char *argv[]) {
    // Check whether we have valid number of parameters
    if (argc != 2) {
        printf("Provide number of pirates.\n");
        exit(-1);
    }
    
    pirates = atoi(argv[1]);
    // Stopping program on getting CTRL+C, releasing memory and semaphore
    signal(SIGINT, sigint_handler);
    
    signal(SIGUSR1, stop);
    // Building semaphore and shared memory
    key_t key = ftok("sem_file", 1);
    semid = semget(key, 1, 0644 | IPC_CREAT);
    semctl(semid, 0, SETVAL, 0);
    
    
    key = ftok("shm_file", 1);
    shmid = shmget(key, sizeof(island), 0644 | IPC_CREAT);
    shm = (island*) shmat(shmid, NULL, 0);
    
    for (int i = 0; i < ISLAND_AREAS; ++i) {
        shm->areas[i] = -1;
    }
    
    srand(time(NULL));
    struct sembuf buf;
    shm->treasure = rand() % 15;
    printf("Treasure is burried in area: %d\n", shm->treasure);

    pid_t pid;
    captain = getpid();
    
    // Building child processes that are simulating pirates
    for (int i = 1; i <= pirates; ++i) {
        pid = fork();
        if (pid < 0) {
            printf("Fork error!\n");
            exit(-1);
        } else if(pid == 0) {
            char str1[sizeof(int)], str2[sizeof(int)];
            sprintf(str1, "%d", i);
            sprintf(str2, "%d", captain);
            execl("./pirate", "./pirate", str1, str2, NULL);
            exit(0);
        } else {
            processes[i-1] = pid;
        }
    }

    for (int i = 0; i < pirates; ++i) {
        buf.sem_num = 0;
        buf.sem_op = 1;
        buf.sem_flg = 0;
        semop(semid, &buf, 1);
    }

    // Wait for pirate processes
    pause();
}
