/*
 * join1.c
 */

#include <syscall.h>

void p2(char *s){
  Exit(0);
}


void p1(char *s){
  Fork(p2);
  Exit(0);
}

main()
{
  int pid1, pid2, pid3;

  pid1 = Fork(p1);
  pid2 = Fork(p1);
  pid3 = Fork(p1);
  
  Join(pid1);
  Join(pid2);
  Join(pid3);  

  //Halt;
}
