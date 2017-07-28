#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main(void)
{
	for(int i = 0; i < 5; i++)
		printf("%d\n", pow(2, rand()%4));
}
