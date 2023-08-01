#include <stdio.h>      // sprintf
#include <stdlib.h>     // exit
#include <string.h>     // strlen
#include <unistd.h>     // write, NULL, close, exit
#include <netinet/in.h> // sockaddr_in
#include <sys/select.h> // select
#include <sys/types.h>  // size_t
#include <sys/socket.h> // sockaddr, socklen_t, socket, bind, listen, accept, recv, send

// structures
typedef struct  s_cli {
    int fd;
    int id;
    struct s_cli * next;
}               t_cli;

// global variables
int         sfd = 0;
t_cli *     cli = NULL;
int         max_fd = -1;
fd_set      afds, rfds, wfds;
int         buf_size = 1000;
char        buf[1000];

// functions
void    ft_err();
int     add_cli(int fd);
int     rmv_cli(int fd);
void     send_to_others(char *line, int fd);

void    ft_err()
{
    t_cli *tmp = cli;
    while (tmp != NULL){
        t_cli *next = tmp->next;
        rmv_cli(tmp->fd);
        tmp = next;
    }
    if (sfd > 0)
        close(sfd);
    write(2, "Fatal error\n", strlen("Fatal error\n")); 
    exit(1);     
}

int    add_cli(int fd)
{
    int id = 0;
    t_cli * last = NULL;
    for (t_cli *tmp = cli; tmp != NULL; tmp = tmp->next){
        id = tmp->id + 1;
        last = tmp;
    }
    t_cli * new_cli = (t_cli *) malloc(sizeof(t_cli));
    if (new_cli == 0)
        ft_err("add_cli");
    new_cli->fd = fd;
    new_cli->id = id;
    new_cli->next = NULL;
    if (max_fd < fd)
        max_fd = fd;
    if (last == NULL)
        cli = new_cli;
    else
        last->next = new_cli;
    FD_SET(new_cli->fd, &afds);
    return (new_cli->id);
}

int rmv_cli(int fd){
    int     id = 0;
    t_cli * last = NULL;
    for (t_cli *tmp = cli; tmp != NULL; tmp = tmp->next){
        if (tmp->fd == fd) {
            close(tmp->fd);
            FD_CLR(tmp->fd, &afds);
            id = tmp->id;
            if (last == NULL)
                cli = cli->next;
            else
                last->next = tmp->next;
            free(tmp);
            tmp = 0;
            break;
        }
        last = tmp;
    }
    max_fd = sfd;
    for (t_cli *tmp = cli; tmp != NULL; tmp = tmp->next){
        if (max_fd < tmp->fd)
            max_fd = tmp->fd;
    }
    return (id);
}

void send_to_others(char *line, int fd)
{
    for (t_cli *tmp = cli; tmp != NULL; tmp = tmp->next){
        if (FD_ISSET(tmp->fd, &wfds))
        {
            if (tmp->fd != fd){
                if (send(tmp->fd, line, strlen(line), 0) < 0)
                    ft_err("send_to_others");                
            }
        }
    }
    bzero(buf, buf_size);
}

int main(int ac, char **av) {
	socklen_t           addr_size;
	struct sockaddr_in  serv_addr, cli_addr; 

    if (ac != 2){
        write(2,"Wrong number of arguments\n", strlen("Wrong number of arguments\n"));
        exit(1);
    }
	
    sfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); 
	if (sfd == -1)
        ft_err("socket");
	
    addr_size = sizeof(cli_addr);
    bzero(&serv_addr, addr_size); 
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(atoi(av[1])); 
	serv_addr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	if ((bind(sfd, (const struct sockaddr *)&serv_addr, addr_size)) != 0)
        ft_err("bind");
	if (listen(sfd, 10) != 0)
        ft_err("listen");
    
    FD_ZERO(&afds);
    FD_SET(sfd, &afds);
    max_fd = sfd;
    
    while (1){
        rfds = wfds = afds;
        int ready = select(max_fd + 1, &rfds, &wfds, NULL, NULL);
        if (ready < 0)
            ft_err("select");
        else if (ready == 0){
            continue;            
        }
        else {
            if (FD_ISSET(sfd, &rfds)){
                int cfd = accept(sfd, (struct sockaddr *)&cli_addr, &addr_size);
                if (cfd < 0)
                    ft_err("accept");
                sprintf(buf, "server: client %d just arrived\n", add_cli(cfd));
                send_to_others(buf, cfd);
            }
            else
            {
                t_cli *tmp = cli;
                while (tmp != NULL)
                {
                    if (FD_ISSET(tmp->fd, &rfds)){
                        bzero(buf, buf_size);
                        ssize_t bytes_recv = recv(tmp->fd, &buf, buf_size, 0);
                        if (bytes_recv == -1 || bytes_recv == 0)
                        {
                            t_cli *next = tmp->next;
                            int fd = tmp->fd;
                            sprintf(buf, "server: client %d just left\n", rmv_cli(tmp->fd));
                            send_to_others(buf, fd);
                            tmp = next;
                        }
                        else {
                            int nb_newline = 0;
                            for (int i = 0; buf[i] != 0; i++){
                                if (buf[i] == '\n')
                                    nb_newline++;
                            }
                            int     resp_buf_size = buf_size + ((nb_newline + 1) * strlen("client  : "));//!%d 
                            char    send_buf[resp_buf_size];
                            char    tmp_buf[resp_buf_size];
                            bzero(send_buf, resp_buf_size);
                            bzero(tmp_buf, resp_buf_size);
                            
                            sprintf(send_buf, "client %d: ", tmp->id);

                            for (int i = 0; buf[i] != 0; i++){
                                bzero(tmp_buf, resp_buf_size);
                                sprintf(tmp_buf, "%c", buf[i]);
                                strcat(send_buf, tmp_buf);
                                if (buf[i] == '\n' && buf[i + 1] != 0)
                                {
                                    bzero(tmp_buf, resp_buf_size);
                                    sprintf(tmp_buf, "client %d: ", tmp->id);
                                    strcat(send_buf, tmp_buf);
                                }
                            }
                            send_to_others(send_buf, tmp->fd);
                            tmp = tmp->next;
                        }
                    }
                    else
                        tmp = tmp->next;
                }
            }
        }
    }
}
