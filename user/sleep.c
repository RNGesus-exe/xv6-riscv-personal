#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  if(argc <= 1){
    fprintf(2, "Usage: sleep <ticks>\n");
    exit(1);
  }

  uint ticks = atoi(argv[1]);
  fprintf(1, "Sleeping for %u ticks\n", ticks);
  sleep(ticks);
  exit(0);
}