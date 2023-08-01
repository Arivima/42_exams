#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct s_cli{
	int 	id;
	char 	msg[1024];
}				t_cli;

t_cli	cli[1024];
fd_set	afds, rfds, wfds;
int		max_fd = 0;
int		next_id = 0;
int 	buf_size = 120000;
char 	buf_send[120000];
char 	buf_recv[120000];

void	ft_err(const char *str) {
	if (str)
		write(2, str, strlen(str));
	else
		write(2,"Fatal error", strlen("Fatal error"));
	write(2, "\n", 1);
	exit(1);
}

void	send_others(int fd) {
	for (int fdi = 0; fdi <= max_fd; fdi++)
		if (FD_ISSET(fdi, &wfds) && fd != fdi)
			send(fdi, buf_send, buf_size, 0);
}

int main(int ac, char **av) {
	if (ac != 2)
		ft_err("Wrong number of arguments");

	int sfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sfd <0)
		ft_err(NULL);

	FD_ZERO(&afds);
	bzero(&cli, sizeof(cli));
	max_fd = sfd;
	FD_SET(sfd, &afds);

	struct sockaddr_in serv_addr; 
	socklen_t len = sizeof(serv_addr);
	bzero(&serv_addr, sizeof(serv_addr)); 
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	serv_addr.sin_port = htons(atoi(av[1])); 

	if ((bind(sfd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)  
		ft_err(NULL);
	if (listen(sfd, 10) < 0) 
		ft_err(NULL);

	while (1){
		rfds = wfds = afds;
		if (select(max_fd + 1, &rfds, &wfds, NULL, NULL) < 0)
			continue;
		for (int fdi = 0; fdi <= max_fd; fdi++){
			if (FD_ISSET(fdi, &rfds) && fdi == sfd){
				int cfd = accept(sfd, (struct sockaddr *)&serv_addr, &len);
				if (cfd > 0) {  
					max_fd = max_fd < cfd ? cfd : max_fd;
					cli[cfd].id = next_id++;
					FD_SET(cfd, &afds);
					sprintf(buf_send, "server: client %d just arrived\n", cli[cfd].id);
					send_others(cfd);
					break;
				}
			}
            else if (FD_ISSET(fdi, &rfds) && fdi != sfd) {
				// bzero(&buf_recv, buf_size);
				int r = recv(fdi, buf_recv, 65536, 0);
				if (r <= 0){
					sprintf(buf_send, "server: client %d just left\n", cli[fdi].id);
					send_others(fdi);
					// bzero(&cli[fdi], sizeof(t_cli));
					FD_CLR(fdi, &afds);
					close(fdi);
					break;
				}
				else {
					for (int i = 0, j = strlen(cli[fdi].msg); i < r ; i++, j++){
						cli[fdi].msg[j] = buf_recv[i];
						if (cli[fdi].msg[j] == '\n'){
							cli[fdi].msg[j] = '\0';
							sprintf(buf_send, "client %d: %s\n", cli[fdi].id, cli[fdi].msg);
							send_others(fdi);
                            bzero(&cli[fdi].msg, strlen(cli[fdi].msg));
							j = -1;
						}
					}
					break;
				}
			}
		}
	}
}



