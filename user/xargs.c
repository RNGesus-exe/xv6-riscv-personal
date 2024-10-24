#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

#define BUFFER_SIZE 512

int
main(int argc, char *argv[])
{
    char buf[BUFFER_SIZE];
    char *args[MAXARG];
    int i, n;
    int arg_index = 0;

    if(argc < 2){
        fprintf(2, "Usage: xargs <command> [arguments]\n");
        exit(1);
    }

    for(i = 1; i < argc && i < MAXARG - 1; i++){
        args[arg_index++] = argv[i];
    }

    char line[BUFFER_SIZE];
    int line_index = 0;

    while((n = read(0, buf, sizeof(buf))) > 0){
        for(i = 0; i < n; i++){
            if(buf[i] == '\n'){
                line[line_index] = '\0';

                int input_arg_index = arg_index;
                char *p = line;
                while(*p != '\0' && input_arg_index < MAXARG - 1){
                    while(*p == ' ' || *p == '\t')
                        p++;
                    if(*p == '\0')
                        break;

                    args[input_arg_index++] = p;

                    while(*p != ' ' && *p != '\t' && *p != '\0')
                        p++;
                    if(*p != '\0'){
                        *p = '\0';
                        p++;
                    }
                }
                args[input_arg_index] = 0;

                int pid = fork();
                if(pid < 0){
                    fprintf(2, "xargs: fork failed\n");
                    exit(1);
                } else if(pid == 0){
                    exec(args[0], args);
                    fprintf(2, "xargs: exec failed\n");
                    exit(1);
                } else {
                    wait(0);
                }

                line_index = 0;
            } else {
                if(line_index < BUFFER_SIZE - 1){
                    line[line_index++] = buf[i];
                } else {
                    fprintf(2, "xargs: input line too long\n");
                    exit(1);
                }
            }
        }
    }

    if(n < 0){
        fprintf(2, "xargs: read error\n");
        exit(1);
    }

    exit(0);
}
