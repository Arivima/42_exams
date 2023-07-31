#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <printf.h>

typedef struct  s_cli{
    int     id;
    bool    on;
    bool    resp_mode;
    char    *buf_recv[1024];
    char    *buf_send[1024];
}               t_cli;

int     sfd;
fd_set  rfds, wfds, afds;
t_cli   cli[1024];
int     next_id = 0;
int     max_fd = 0;

void    ft_err(const char * str);
int     rmv_cli(int fd);
void    send_to_others(char *line, int fd);

int main() {
    if (ac != 2)
        ft_err("Wrong number of arguments");
	
    sfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); 
	if (sfd == -1)
        ft_err("socket");
	
	struct sockaddr_in serv_addr; 
    addr_size = sizeof(cli_addr);
    bzero(&serv_addr, addr_size); 
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(atoi(av[1])); 
	serv_addr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	if ((bind(sfd, (const struct sockaddr *)&serv_addr, addr_size)) != 0)
        ft_err("bind");
	if (listen(sfd, 10) != 0)
        ft_err("listen");

    bzero(cli, sizeof(cli * 1024));

    FD_ZERO(&afds)
    FD_SET(sfd, &afds);
    max_fd = sfd;

    while (1){
        rfds = wfds = afds;
        int ready = select(max_fd + 1, &rfds, &wfds, NULL, NULL);
        if (ready < 0)
            ft_err("select");
        else if (ready > 0){
            
            // send message
            for (int i = 0; i <= max_fd ; i++){
                if (cli[i].resp_mode == 1 && FD_ISSET(i, &wfds)){
                    if (send(i, cli[i].buf_send, strlen(cli[i].buf_send), 0) < 0) {
                            close(i);
                            bzero(cli[i], sizeof(cli));
                    }
                    cli[i].resp_mode == 0;
                }
            }

            // new client
            if (FD_ISSET(sfd, &rfds)){
                struct sockaddr_in  cli_addr; 
                socklen_t           addr_size = sizeof(cli_addr); 
                int cfd = accept(sfd, (struct sockaddr *)&cli_addr, &addr_size);
                if (cfd >= 0) {
                    cli[cfd].id = next_id++;
                    cli[cfd].on = 1;
                    max_fd  = max_fd < cfd ? cfd : max_fd ; 
                    FD_SET(cfd, &afds);
                    for (int i = 0; i <= max_fd ; i++){
                        if (cli[i].on && i != cfd){
                            sprintf(cli[i].buf_send, "server: client %d just arrived\n", cli[cfd].id);
                            cli[i].resp_mode = 1;
                        }
                    }
                }
            }

            // new message or client left
            for (int fd = 0; fd <= max_fd ; fd++){
                if (FD_ISSET(fd, &rfds)){
                    ssize_t bytes_read = recv(fd, &(cli[fd].buf_recv), 1024, 0);
                    if (bytes_read <= 0){

                    }
                    else {

                    }

                }
            }

        }
    }
}

void    ft_err(const char * str)
{
    for (int i = 0; i < 1024 ; i++)
        if (cli[i].on)
            close(cli[i].fd);

    if (sfd > 0)
        close(sfd);

    if (str)
        write(2, str, strlen(str)); 
    else
        write(2, "Fatal error", strlen("Fatal error")); 
    write(2, "\n", strlen("\n")); 
    exit(1);     
}
