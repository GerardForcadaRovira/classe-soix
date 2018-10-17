#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX 100000

union semun{
    int val;
    struct semid_ds* buf;
    unsigned short* array;
}

int main(){

    int shmID = shmget(1234,sizeof(int),IPC_CREAT | 0600);
    if(shmID<0){
        std::cout<<"error en reservar memoria SHM"<<std::endl;
        exit(1);
    }
    int* shmPTR = (int*)shmat(shmID,NULL,0);
    if(shmPTR<0){
        std::cout<<"error en reservar memoria SHM ptr"<<std::endl;
        exit(2);
    }
    *shmPTR=0;

    ///RESERVAR MEMORIA PARA EL SEMAFORO
    int semID = semget(5678, 1, IPC_CREAT | 0600);
    if(semID<0){
        std::cout<<"error en reservar memoria SEMAFORO"<<std::endl;
        exit(3);
    }

    ///INIT SEMAFORO PARA UN PROCESO A LA VEZ
    union semun arg;
    arg.val = 1;
    int result = semctl(semID, 0, SETVAL, arg);
    if(result<0){
        std::cout<<"error semctl"<<std::endl;
        exit(4);
    }

    sembuf opSem;
    opSem.sem_flg = SEM_UNDO;
    opSem.sem_num = 0;

    for(int i =0;i<4;i++){
        pid_t pid=fork();
        if(pid==0){
            for(int j = ; j<MAX;j++){
            ///WAIT
                opSem.sem_op = -1;
                result = semop(semID, &opSem, 1);
                    if(result<0){
                        std::cout<<"error WAIT"<<std::endl;
                        exit(4);
                    }
                (*shmPTR)++;
            ///SIGNAL
                opSem.sem_op = 1;
                result = semop(semID, &opSem, 1);
                    if(result<0){
                        std::cout<<"error SIGNAL"<<std::endl;
                        exit(4);
                    }
            }
            shmdt(shmPTR);
            exit(0);
        }

    }///for hijos

    for(int i = 0; i<4;i++){
        int status;
        wait(&status);
    }
    std::cout<<"El resultado final"<<shmPTR<<std::endl;
    shmdt(shmPTR);
    shmctl(shmID, IPC_RMID, NULL);
    shmctl(shmID, 0, IPC_RMID, arg);
    return 0;
}
