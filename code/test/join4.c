/*
 * join1.c
 */

#include <syscall.h>

void p(char *s){
  Exit(0);
}

main()
{
  int pid1, pid2, pid3;

  pid1 = Fork(p);
  pid2 = Fork(p);
  pid3 = Fork(p);
  
  Join(pid1);
  Join(pid2);
  Join(pid3);
  
  Halt();
}
