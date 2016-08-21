#include <stdio.h>
#include <errno.h>

#include "ComLib.h"

char	errmsg[MAX_ERRO_LEN+1];

int main(int agrs, char * argv[]) {
	if(agrs != 3) {
		printf("Usage : %s filename copyfilename\n", basename(argv[0]));
		return(-1);
	}

	CopyFile(argv[1], argv[2]);

	return(0);
}
