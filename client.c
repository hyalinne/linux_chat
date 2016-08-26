#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXLEN 1024

int main(int args, char * argv[]) {
	if(args != 3) {
		printf("Usage : %s [address] [port] ", basename(argv[0]));
		return -1;
	}

	int svr;
	struct sockaddr_in svr_addr;
	
	int nfds;
	fd_set read_fds;

	char recvline[MAXLEN];
	char sendline[MAXLEN];
	char syst[MAXLEN];
	int n;

	if((svr = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket error : ");
	}

	bzero((char *)&svr_addr, sizeof(svr_addr));
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = inet_addr(argv[1]);
	svr_addr.sin_port = htons(atoi(argv[2]));

	if(connect(svr, (struct sockaddr *)&svr_addr, sizeof(svr_addr)) < 0) {
		perror("Connect error : ");
		return -1;
	}

	send(svr, "c", 1, 0);
	printf("Connect success.\n");

	nfds = svr + 1;
	FD_ZERO(&read_fds);

	while(1) {
		FD_SET(0, &read_fds);
		FD_SET(svr, &read_fds);

		if(select(nfds, &read_fds, (fd_set *)0, (fd_set *)0, 0) < 0) {
			perror("Select error : ");
			return -1;
		}
		
		if(FD_ISSET(svr, &read_fds)) {
			if((n = recv(svr, recvline, MAXLEN, 0)) > 0) {
				recvline[n] = '\0';	
				printf("%s", recvline);
			}
		}
	
		if(FD_ISSET(0, &read_fds)) {
			if(read(0, sendline, MAXLEN) > 0) {
				if(strncmp(sendline, "/j", 2) == 0) {
					int i;
					char rn[10];
					for(i = 0; i < strlen(sendline)-4; i++) {
						rn[i] = sendline[i+3];
					}
					rn[strlen(sendline)-4] = '\0';
					sprintf(syst, "./client 127.0.0.1 %s", rn);
					system(syst);
				}
				send(svr, sendline, strlen(sendline), 0);
			}
		}
	}
}
