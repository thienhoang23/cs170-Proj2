/*
 * open-test.c
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
	int openFileId1, openFileId2, openFileId3;

	openFileId1 = Open("open1.txt");
	openFileId2 = Open("open2.txt");
	openFileId3 = Open("open3.txt");
	Halt();
}
