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
	char *writeBuf = "Indubitably";
	int OpenFileId;
	Create("proj2-test-file.txt");
	OpenFileId = Open("proj2-test-file.txt");
	print("Writing said file...\n");
	Write(writeBuf, 11, OpenFileId);
	Halt();
}
