#include "kernel/types.h"
#include "user/user.h"

void primes(int p_left[2]) __attribute__((noreturn));

void primes(int p_left[2]){
    close(p_left[1]);

    int prime;
    if(read(p_left[0], &prime, sizeof(int)) == 0){
        close(p_left[0]);
        exit(0);
    }

    printf("prime %d\n", prime);

    int p_right[2];
    int status = pipe(p_right);
    if (status < 0) {
        fprintf(2, "%d: pipe failed with error code: %d\n", getpid(), status);
        close(p_left[0]);
        exit(1);
    }

    int pid = fork();
    if(pid < 0){
        fprintf(2, "fork failed\n");
        close(p_left[0]);
        close(p_right[0]);
        close(p_right[1]);
        exit(1);
    }
    else if(pid == 0){
        close(p_left[0]);
        close(p_right[1]);
        primes(p_right);
    }
    else{
        close(p_right[0]);

        int num;
        while (read(p_left[0], &num, sizeof(int)) != 0) {
            if (num % prime != 0) {
                write(p_right[1], &num, sizeof(int));
            }
        }

        close(p_left[0]);
        close(p_right[1]);
        wait(0);
        exit(0);
    }
}

int main(int argc, char *argv[]){
    if(argc > 1){
    fprintf(2, "Usage: primes\n");
    exit(1);
    }

    int p[2];
    int status;
    status = pipe(p);

    if(status < 0){
        fprintf(2, "pipe failed with error code: %d\n", status);
        exit(1);
    }

    int pid = fork();
    if(pid < 0){
        fprintf(2, "fork failed\n");
        close(p[0]);
        close(p[1]);
        exit(1);
    }
    else if(pid == 0){
        close(p[1]);
        primes(p);
    }
    else{
        close(p[0]);

        for(int i=2; i<=280; ++i){
            if(write(p[1], &i, sizeof(int)) != sizeof(int)){
                fprintf(2, "write failed\n");
                close(p[1]);
                wait(0);
                exit(1);
            }
        }

        close(p[1]);
        wait(0);
        exit(0);
    }
}