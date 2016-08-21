#include <stdio.h>
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

// fileLock
	FILE * f;
//	f = fopen(argv[1],"r");
	
	if(LockFile("./abc") == 0) {
		printf("Success\n");
	}

	while(1) {
		char c;
		printf("Enter s\n");
		scanf("%c", &c);
		if(c == 's') {
			break;
		}
	}
// fileUnlock
	//unLockFile("./abc");
//	fclose(f);

	return(0);
}
