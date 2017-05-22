/*
 * close-test.c
 */

#include <syscall.h>

void print(char *s)
{
	int len = 0;

	while (*s++)
		++len;

	Write(s-len-1, len, 1);
}

main()
{
	int openFileId = Open("close1.txt");
	Close(openFileId);
	Halt();
}
