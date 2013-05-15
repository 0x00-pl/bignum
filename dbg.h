#include <stdio.h>
#include <unistd.h>

//#define pldbg

void dbgmsg(const char* msg){
  printf("%s",msg);
}

void tat(int boolean, const char* msg){
  if(boolean==0){
    printf("%s",msg);
    sleep(3);
  }
}

