#include <stdio.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[])
{
	int x = 0;
	clock_t start = clock(); // start time

	printf("--------------------------------------------------\n");
	printf("Test program #2\n\n");

	for (int i = 1; i <= 100; i++) 
	{
		x+=i;
	}
	
	printf("Summation from 1 to 100 = %d \n", x);

        clock_t end = clock(); // finish time

        printf("\nTime: %lf\n", (double)(end - start)/CLOCKS_PER_SEC);
	printf("--------------------------------------------------\n");

        return 0;
}
