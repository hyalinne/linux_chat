#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#include "ComLib.h"

char	errmsg[MAX_ERRO_LEN+1];

int main(int agrs, char * argv[]) {
/*
	if(agrs != 2) {
		printf("Usage : %s filename\n", basename(argv[0]));
		return(-1);
	}
*/
	while(1) {
		if(LockFile("./abc") == 0) {
			break;
		}
		sleep(1);
		printf("1\n");
	}

//	unLockFile("./abc");

	FILE * f;
	f = fopen("./abc", "r");							

	while(!feof(f)) {
		printf("%c", fgetc(f));
	}

	fclose(f);

	return(0);
}
