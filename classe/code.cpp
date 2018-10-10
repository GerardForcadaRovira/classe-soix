#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
/*
    int des1 = 100;
    int des2 = 250;
    int con1 = 50;
    int con2 = 25;
*/


int main(){

    int shmID = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    int* hpEstructura = (int)shmat(shmID,Null,0);
    *hpEstructura = 10000;
    do{
        if(fork()==0){
            //hijo des1
            *hpEstructura-=100;
        }else{
            if(fork()==0){
                //hijo des2
                *hpEstructura-=250;
            }else{
                if(fork()==0){
                    //hijo con1
                    *hpEstructura+=50;
                }else{
                    //padre con2
                    *hpEstructura+=25;
                }

            }

        }
        std::cout<< *hpEstructura <<std::endl;
    }while(hpEstructura>0)
    int status;
    wait(&status);
    shmdt(hpEstructura);
    shmctl(shmID,IPC_RMID,NULL);

    return 0;
}//compiler sudo apt-get install g++ en terminal
