#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <wait.h>

#define	MAXLEN	1024

struct client {
	int desc;
	char name[20];
	struct client * next;
};

struct room {
	int desc;
	int room_no;
	char name[50];
	struct room * next;
};

void add_client(int svr); // both room & client 
void remove_client(struct client * prev, struct client * temp);
void remove_room(int room_no);
void command(struct client * prev, struct client * temp, char * rline, int n);
void make_room(int room_no);

struct client * first, * last;
struct room * froom = NULL, * lroom = NULL;
int num_cli = 0;
int num_room = 0;
pid_t pid;

char * port;

int main(int args, char * argv[]) {
	if(args < 2) {
		printf("Usage : %s [port number]\n", argv[0]);
		return -1;
	}
	port = argv[1];

	int	svr;	// server	
	struct client * temp, * prev;


	struct	sockaddr_in	svr_addr;	// server socket set

	int check;

	char rline[MAXLEN];
	char msg[MAXLEN];

	fd_set	read_fds;
	int	nfds;

	int i, max;
	
	if((svr = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket error\n");
		return -1;
	}

	bzero((char *)&svr_addr, sizeof(svr_addr));
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	svr_addr.sin_port = htons(atoi(argv[1]));

	if(bind(svr, (struct sockaddr *)&svr_addr, sizeof(svr_addr)) < 0) {
		perror("bind error : ");
		return -1;
	}


	if(listen(svr, 5) < 0) {
		printf("listen error\n");
		return -1;
	}

	nfds = svr + 1;

	while(1) {
		FD_ZERO(&read_fds);
		FD_SET(svr, &read_fds);

		if(num_cli-1 >= 0) {
			max = last->desc;
			if(lroom != NULL) {
			}
			temp = first;
			for(i = 0; i < num_cli; i++) {
				if(temp->desc > max) max = temp->desc;
				FD_SET(temp->desc, &read_fds);
				temp = temp->next;
			}
			nfds = max + 1;
		}

		if(select(nfds, &read_fds, (fd_set *)0, (fd_set *)0, 0) < 0) {
			perror("select error");
			return -1;
		}

		if(FD_ISSET(svr, &read_fds)) {
			add_client(svr);
		}

		temp = first;
		for(i = 0; i < num_cli; i++) {
			if((check = FD_ISSET(temp->desc, &read_fds)) > 0) {
				int n = recv(temp->desc, rline, MAXLEN, 0);
				if(n == 0) {
					remove_client(prev, temp);
					break;
				} else if(n > 0) {
					rline[n] = '\0';
					printf("%d : %s", temp->desc, rline);
// command
					if(rline[0] == '/') {
						command(prev, temp, rline, n);
						continue;
					}
				}
			}
			prev = temp;
			temp = temp->next;
		}
	}

	close(svr);
	return 0;
}

void add_client(int svr) {
	int len, cli;
	struct sockaddr_in cli_addr;
	struct client * new;
	struct room * nr; // room

	char msg[MAXLEN];

	len = sizeof(cli_addr);
	cli = accept(svr, (struct sockaddr *)&cli_addr, &len);

	if(cli != -1) {
		if(recv(cli, msg, MAXLEN, 0) > 0) {
			if(strncmp(msg, "c", 1) == 0) {
				new = (struct client *)malloc(sizeof(struct client));
				new->desc = cli;
				new->next = NULL;
				sprintf(new->name, "%d", cli);
				if(num_cli == 0) {
					first = last = new;
				} else {
					last->next = new;
					last = new;
				}
				printf("Add new client : %s\n", new->name);
				num_cli++;
			} else if(strncmp(msg, "r", 1) == 0) {
				nr = (struct room *)malloc(sizeof(struct room));
				nr->desc = cli;
				nr->next = NULL;
				memset(msg, "0x00", MAXLEN);
				if(recv(cli, msg, MAXLEN, 0) > 0) {
					nr->room_no = atoi(msg);
				}
				if(num_room == 0) {
					froom = lroom = nr;
				} else {
					lroom->next = nr;
					lroom = nr;
				}
				printf("Add new room : %d\n", nr->room_no);
				num_room++;
			}
		}
	}
}

void remove_client(struct client * prev, struct client * temp) {
	char msg[MAXLEN];

	close(temp->desc);
	if(temp == first) {
		first = temp->next;
	} else if(temp == last) {
		last = prev;
		last->next = NULL;
	} else {
		prev->next = temp->next;	
	}
	num_cli--;
}

void remove_room(int room_no) {
	struct room * prev = froom;
	struct room * tmp = froom;
	while(tmp->next != NULL) {
		if(tmp->room_no == room_no) {
			break;
		}
		prev = tmp;
		tmp = tmp->next;
	}
	if(tmp->room_no == room_no) {
		close(tmp->desc);
	}
	if(lroom->room_no == room_no) {
		prev->next = NULL;
		lroom = prev;	
	}
	num_room--;
}

void command(struct client * prev, struct client * temp, char * rline, int n) {
	int i, e;
	char rn[10]; // room number
	char msg[MAXLEN];
	char wn[20] = {}; // whisper receiver name
	
	struct client * tmp;

	if(rline[1] == 'q') {
		remove_client(prev, temp);
	} else if(rline[1] == 'n') {
		memset(temp->name, NULL, sizeof(temp->name));
		for(i = 0; i < n-5; i++) {
			temp->name[i] = rline[i+3];
		}
		temp->name[n-5] = '\0';
	} else if(rline[1] == 'h') {
		sprintf(msg, "q : quit\nn [name] : change your nickname\nm [room number] : make chat room\nj [room number] : join chat room\n");
		send(temp->desc, msg, strlen(msg), 0);
	} else if(rline[1] == 'm') {
		for(i = 0; i < n-4; i++) {
			rn[i] = rline[i+3];	
		}
		rn[n-4] = '\0';
		make_room(atoi(rn));
	} else {
		sprintf(msg, "h : show command\n");
		send(temp->desc, msg, strlen(msg), 0);
	}
}

void make_room(int room_no) {
	char msg[MAXLEN];
	int status;
	pid_t pid_child;

	sprintf(msg, "./chat 127.0.0.1 %s %d", port, 10000 + room_no);
	pid = fork();
	if(pid  < 0) {
		perror("fork error : ");
	} else if(pid == 0) {
		printf("%d room is made.\n", room_no);
		system(msg);
	} else {
		pid_child = waitpid(pid, &status, WNOHANG);
		if(0 != pid_child) {
			printf("Room close.\n");
		}	
	}
}
