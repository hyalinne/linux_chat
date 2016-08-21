#include <stdio.h>
#include <signal.h>

#include "ComLib.h"

char	errmsg[MAX_ERRO_LEN+1];

int state = 1; // 1 = +,  2 = -, 0 = exit
void sig_plus();
void sig_minus();
void sig_quit();

int main() {
	Signal(SIGQUIT, sig_quit);
	Signal(SIGUSR1, sig_plus);
	Signal(SIGUSR2, sig_minus);

	if(fork() > 0) exit(0);
	close(0);
	close(2);
	setsid();

	int sum = 0;
	printf("%d\n", sum);
	while(1) {
		sleep(1);
		if(state == 1) {
			sum += 1;
			printf("%d\n", sum);
		} else if(state == 2) {
			sum -= 1;
			printf("%d\n", sum);
		} else if(state == 0) {
			break;
		}
	}
	printf("I Will Be Back.\n");
	return 0;
}


void sig_plus() {
	state = 1;
	return;
}

void sig_minus() {
	state = 2;
	return;
}

void sig_quit() {
	state = 0;
	return;
}

