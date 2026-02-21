#include <stdio.h>
extern long Sub2(short, long);
extern char StringInASM[];
main()
{
	printf("Link C module and %s together\n%ld\n", StringInASM, Sub2(50, 31));
}