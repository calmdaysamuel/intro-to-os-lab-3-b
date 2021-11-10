#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
void ClientProcess(int[]);
void ParentProcess(int SharedMem[]);
int main(int argc, char *argv[])
{
    int ShmID;
    int *ShmPTR;
    pid_t pid;
    int status;

//     if (argc != 3)
//     {
//         printf("Use: %s #1 #2\n ", argv[0]);
//         exit(1);
//     }

    ShmID = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
    if (ShmID < 0)
    {
        printf("*** shmget error (server) ***\n");
        exit(1);
    }
    printf("Server has received a shared memory of four integers...\n");

    ShmPTR = (int *)shmat(ShmID, NULL, 0);
    if (*ShmPTR == -1)
    {
        printf("*** shmat error (server) ***\n");
        exit(1);
    }
    printf("Server has attached the shared memory...\n");

    ShmPTR[0] = 0; //BankAccount
    ShmPTR[1] = 0;             //Turn
    printf("Server has filled %d %d in shared memory...\n",
           ShmPTR[0], ShmPTR[1]);

    printf("Server is about to fork a child process...\n");
    pid = fork();
    if (pid < 0)
    {
        printf("*** fork error (server) ***\n");
        exit(1);
    }
    else if (pid == 0)
    {
        for (int i = 0; i < 25; i++)
        {
            ClientProcess(ShmPTR);
        }

        exit(0);
    }
    for (int i = 0; i < 25; i++)
    {
        ParentProcess(ShmPTR);
    }

    wait(&status);
    printf("Server has detected the completion of its child...\n");
    shmdt((void *)ShmPTR);
    printf("Server has detached its shared memory...\n");
    shmctl(ShmID, IPC_RMID, NULL);
    printf("Server has removed its shared memory...\n");
    printf("Server exits...\n");
    exit(0);
}

void ParentProcess(int SharedMem[])
{
    sleep(random() % 6);
    int bankaccount = SharedMem[0];

    while (SharedMem[1] != 0)
    {
    }
    if (bankaccount <= 100)
    {
        int chance = random() % 100;
        if (chance % 2 == 0)
        {
            bankaccount += chance;
            printf("Dear old Dad: Deposits $%d / Balance = $%d\n", chance, bankaccount);
        }
        else
        {
            printf("Dear old Dad: Doesn't have any money to give\n");
        }
    }

    SharedMem[0] = bankaccount;
    SharedMem[1] = 1;
}
void ClientProcess(int SharedMem[])
{
    sleep(random() % 6);
    int bankaccount = SharedMem[0];

    while (SharedMem[1] != 1)
    {
    }
    int chance = random() % 50;
    printf("Poor Student needs $%d\n", chance);
    if (chance <= bankaccount)
    {
        bankaccount -= chance;
        printf("Poor Student: Withdraws $%d / Balance = $%d\n", chance, bankaccount);
    }
    else
    {
        printf("Poor Student: Not Enough Cash ($%d)\n", bankaccount);
    }

    SharedMem[0] = bankaccount;
    SharedMem[1] = 0;
}