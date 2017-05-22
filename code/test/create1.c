/*
 * create-test.c
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
	Create("proj2-test-file.txt");
	Halt();
}
