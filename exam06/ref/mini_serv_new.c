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
    char    *msg[1024];
}               t_cli;

int     sfd;
fd_set  rfds, wfds, afds;
t_cli   cli[1024];              //1024 = max fd per process on linux
int     next_id = 0;
int     max_fd = 0;             // will be increased but not decreased 
char    *buf_recv[120000];      // 120 000 why ?
char    *buf_send[120000];
int     buf_size = 120000;

void    ft_err(const char * str);
void    ft_send_others(int fd);

#include <sys/time.h>
#include <sys/resource.h>


int main() {

    struct rlimit l;
    getrlimit(RLIMIT_NOFILE, &l)
    printf("soft limit : %d\nhard limit:%d\n", l.rlim_cur, l.rlim_max);


    if (ac != 2)
        ft_err("Wrong number of arguments");
	
    sfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); //!
	if (sfd == -1)
        ft_err(NULL);
	
	struct sockaddr_in serv_addr; 
    addr_size = sizeof(cli_addr);
    bzero(&serv_addr, addr_size); 
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(atoi(av[1])); 
	serv_addr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1

	if ((bind(sfd, (const struct sockaddr *)&serv_addr, addr_size)) != 0)
        ft_err(NULL);
	if (listen(sfd, 10) != 0)
        ft_err(NULL);

    bzero(&cli, sizeof(cli));

    FD_ZERO(&afds)
    FD_SET(sfd, &afds);
    max_fd = sfd;

    while (1){
        rfds = wfds = afds;
        int ready = select(max_fd + 1, &rfds, &wfds, NULL, NULL);
        if (ready < 0)
            ft_err(NULL); // continue;
        else if (ready > 0){
            for (int fdi = 0; fdi <= max_fd ; fdi++){
                if (fdi == sfd && FD_ISSET(sfd, &rfds)) /*new cli*/ {
                    struct sockaddr_in  cli_addr; 
                    socklen_t           addr_size = sizeof(cli_addr); 
                    int cfd = accept(sfd, (struct sockaddr *)&cli_addr, &addr_size);
                    if (cfd >= 0) {
                        cli[cfd].id = next_id++;
                        cli[cfd].on = 1;
                        max_fd  = max_fd < cfd ? cfd : max_fd ; 
                        FD_SET(cfd, &afds);
                        sprintf(buf_send, "server: client %d just arrived\n", cli[cfd].id);
                        ft_send_others(cfd);
                        break;
                    }
                }
                else if (fdi != sfd && FD_ISSET(fdi, &rfds)) {
                    ssize_t r = recv(fdi, &buf_recv, buf_size, 0);
                    if (r <= 0) /*cli left*/ {
                        ft_close(fdi);
                        break;
                    }
                    else /*new msg*/ {
                        for (int i = 0, j = strlen(cli[fdi].msg; i < r; i++, j++)){
                            cli[fdi].msg[j] = buf_recv[i];
                            if (cli[fdi].msg[j] == '\n'){
                                cli[fdi].msg[j] == '\0';
                                sprintf(buf_send, "client %d: %s\n", cli[fdi].id, cli[fdi].msg);
                                ft_send_others(fdi);
                                bzero(&cli[fdi].msg, strlen(cli[fdi].msg));
                                j = -1; //bc incrementation
                            }

                        }
                        break;
                    }
                }
            }
        }
    }
}

void    ft_close(int fd)
{
    close(fd);
    FD_CLR(fd, &afds);
    bzero(cli[fd], sizeof(t_cli));
    sprintf(buf_send, "server: client %d just left\n", cli[fd].id);
    ft_send_others(fd);
}

void    ft_err(const char * str)
{
    for (int i = 0; i < 1024 ; i++)
        if (cli[i].on)
            close(i);

    if (sfd > 0)
        close(sfd);

    if (str)
        write(2, str, strlen(str)); 
    else
        write(2, "Fatal error", strlen("Fatal error")); 
    write(2, "\n", strlen("\n")); 
    exit(1);     
}

void    ft_send_others(int fd){
    int     fail_fd[max_fd + 1];
    bzero(&fail_fd, sizeof(fail_fd));
    for (int fdi = 0; fdi <= max_fd ; fdi++){
        if (fd != fdi && FD_ISSET(fdi, &wfds)){
            if (send(fdi, buf_send, buf_size, 0) < 0){
                close(fdi);
                FD_CLR(fdi, &afds);
                bzero(cli[fdi], sizeof(t_cli));
                fail_fd[fdi] = 1;
            }
        }
    }
    for (int fdi = 0; fdi <= max_fd ; fdi++) {
        if (fail_fd[fdi] == 1){
            sprintf(buf_send, "server: client %d just left\n", cli[fd].id);
            ft_send_others(fd);
        }
    }
}
