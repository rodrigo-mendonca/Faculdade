#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main (int argc, char *argv[])
{


	clock_t oClock = clock();
	// Rotina
	float nNow = (float)(clock() - oClock) / CLOCKS_PER_SEC;
	printf("\n\n%.4f Segundos\n\n",nNow);

	double Start = omp_get_wtime();
	// Rotina
	double End = omp_get_wtime() - Start;
	printf("\n\n%g Segundos",End);
}
