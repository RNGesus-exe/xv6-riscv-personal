/*
    1) First parent writes to the pipe
    2) Child receives the byte and prints ping + pid
    3) Child writes to the pipe
    4) Parent receives the byte and prints pong + pid
*/

#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if(argc > 1){
    fprintf(2, "Usage: pingpong\n");
    exit(1);
    }

    int ping_pipe[2]; // Parent to child
    int pong_pipe[2]; // Child to Parent
    if(pipe(ping_pipe) < 0|| pipe(pong_pipe) < 0){
        fprintf(2, "pipe() failed\n");
        exit(1);
    }   

    int pid = fork();
    if(pid < 0){
        fprintf(2, "fork() failed\n");
        close(ping_pipe[1]);
        close(pong_pipe[1]);
        close(ping_pipe[0]);
        close(pong_pipe[0]);
        exit(1);
    }

    if(pid == 0){
        // Child Process

        close(ping_pipe[1]);
        close(pong_pipe[0]);

        char buf[1];
        if(read(ping_pipe[0], buf, 1) != 1){
            fprintf(2, "%d: child read failed\n", getpid());\
            exit(1);
        }
        fprintf(1, "%d: received ping\n", getpid());

        if(write(pong_pipe[1], buf, 1) != 1){
            fprintf(2, "%d: child write failed\n", getpid());
            exit(1);
        }
        
        
        close(ping_pipe[0]);
        close(pong_pipe[1]);

        exit(0);
    }
    else{
        // Parent Process
        close(ping_pipe[0]);
        close(pong_pipe[1]);
        
        char buf[1] = {'X'};
        if(write(ping_pipe[1], buf, 1) != 1){
            fprintf(2, "%d: parent read failed\n", getpid());
            exit(1);
        }
        
        if(read(pong_pipe[0], buf, 1) != 1){
            fprintf(2, "%d: parent read failed\n", getpid());
            exit(1);
        }
        fprintf(1, "%d: received pong\n", getpid());

        close(ping_pipe[0]);
        close(pong_pipe[1]);

        wait(0);

        exit(0);
    }
}