#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define	MAXLEN	1024

struct client {
	int desc;
	char name[20];
	struct client * next;
};

struct room {
	int room_no;
	char name[50];
	struct room * next;
};

int add_client(int svr, int num_cli);
void remove_client(struct client * prev, struct client * temp);
int command(struct client * prev, struct client * temp, char * rline, int n, int num_cli);
void make_room(int room_no);
void print_command(int desc);

struct client * first, * last;
struct client * froom = NULL, * lroom = NULL;
pid_t pid;

int main(int args, char * argv[]) {
	if(args < 2) {
		printf("Usage : %s [port number]\n", argv[0]);
		return -1;
	}

	int	svr;	// server	
	struct client * temp, * prev;


	int num_cli = 0;
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
			num_cli = add_client(svr, num_cli);
		}

		temp = first;
		for(i = 0; i < num_cli; i++) {
			if((check = FD_ISSET(temp->desc, &read_fds)) > 0) {
				int n = recv(temp->desc, rline, MAXLEN, 0);
				if(n == 0) {
					remove_client(prev, temp);
					num_cli--;
					break;
				} else if(n > 0) {
					rline[n] = '\0';
// command
					if(rline[0] == '/') {
						num_cli = command(prev, temp, rline, n, num_cli);
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

int add_client(int svr, int num_cli) {
	int len, cli;
	struct sockaddr_in cli_addr;
	struct client * new;

	char msg[MAXLEN];

	len = sizeof(cli_addr);
	cli = accept(svr, (struct sockaddr *)&cli_addr, &len);

	if(cli != -1) {
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
	}

	return num_cli+1;
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
}

void add_room(int room_no) {
	struct room * new;

	/*
	new = (struct room *)malloc(sizeof(struct room));
	new->next = NULL;
	sprintf(new->name, "%d", cli);
	if(froom == NULL) {
		froom = lroom = new;
	} else {
		lroom->next = new;
		lroom = new;
	}
	*/
}

void remove_room(struct room * temp) {
	/*
	struct room * tmp = first;
	while(tmp->next != NULL) {
		if(tmp == temp) {
			break;
		}
	}

	if(temp == first) {
		first = temp->next;
	} else if(temp == last) {
		last = prev;
		last->next = NULL;
	} else {
		prev->next = temp->next;	
	}
	*/
}

int command(struct client * prev, struct client * temp, char * rline, int n, int num_cli) {
	int i, e;
	char rn[10]; // room number
	char msg[MAXLEN];
	char wn[20] = {}; // whisper receiver name
	
	struct client * tmp;

	if(rline[1] == 'q') {
		remove_client(prev, temp);
		return num_cli-1;
	} else if(rline[1] == 'n') {
		memset(temp->name, NULL, sizeof(temp->name));
		for(i = 0; i < n-5; i++) {
			temp->name[i] = rline[i+3];
		}
		temp->name[n-5] = '\0';
	} else if(rline[1] == 'h') {
		print_command(temp->desc);
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
	return num_cli;
}

void make_room(int room_no) {
	char msg[MAXLEN];

	sprintf(msg, "./chat %d", 10000 + room_no);
	pid = fork();
	if(pid  < 0) {
		perror("fork error : ");
	} else if(pid == 0) {
		printf("%d room is made.\n", room_no);
		system(msg);
	}
}


void print_command(int desc) {
	char msg[MAXLEN];

	sprintf(msg, "q : quit\nn [name] : change your nickname\nm [room number] : make chat room\nj [room number] : join chat room\n");
	send(desc, msg, strlen(msg), 0);
}
