#include <stdio.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[])
{
	clock_t start = clock(); // start time

	printf("--------------------------------------------------\n");
	printf("Test program #1\n\n");

	for (int i = 1; i <= 1000000000; i++) 
	{
		if(i%10000000==0) printf("%d  ", i/10000000);
		if(i%100000000==0) printf("\n");
	}

        clock_t end = clock(); // finish time

        printf("\nTime: %lf\n", (double)(end - start)/CLOCKS_PER_SEC);
	printf("--------------------------------------------------\n");

        return 0;
}
