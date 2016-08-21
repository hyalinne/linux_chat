#include <stdio.h>
#include <stdlib.h>

#include "ComLib.h"

char	errmsg[MAX_ERRO_LEN + 1];

int main()
{
	int sum, i;
	struct timeval * tp;
	tp = (struct timeval *)malloc(sizeof(struct timeval));
		
	GetElapsedTime_Begin(tp);
	for(sum = 0, i = 1; i <= 100000; i++) {
		sum += i;
	}
	printf("Sum : %d, Elapsed Time : %f\n", sum, GetElapsedTime_End(tp));
}
