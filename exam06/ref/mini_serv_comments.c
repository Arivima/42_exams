#include <stdio.h>     // DEBUG
#include <stdlib.h>     // exit
#include <string.h>     // strlen
#include <unistd.h>     // write, NULL, close, exit
#include <netinet/in.h> // sockaddr_in
#include <sys/select.h> // select
#include <sys/types.h>  // size_t
#include <sys/socket.h> // sockaddr, socklen_t, socket, bind, listen, accept, recv, send
#include <fcntl.h>      // fcntl

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
void    ft_err(char * str);
void    print_cli(const char * title);
int     add_cli(int fd);
int     rmv_cli(int fd);
void     send_to_others(char *line, int fd);

void    ft_err(char * str)
{
    printf("ft_err: error: %s\n", str);
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

void    print_cli(const char * title){
    printf("| print_cli() | %s | max_fd %d\n", title, max_fd);
    for (t_cli *tmp = cli; tmp != NULL; tmp = tmp->next){
        printf("| client | id : %d | fd : %d | next %p\n", tmp->id, tmp->fd, tmp->next);
    }
    if (cli == NULL)
    printf("| no clients\n");
}

int    add_cli(int fd)
{
    // record last node ptr and id
    int id = 0;
    t_cli * last = NULL;
    for (t_cli *tmp = cli; tmp != NULL; tmp = tmp->next){
        id = tmp->id + 1;
        last = tmp;
    }
    // create new node
    t_cli * new_cli = (t_cli *) malloc(sizeof(t_cli));
    if (new_cli == 0)
        ft_err("add_cli");
    new_cli->fd = fd;
    new_cli->id = id;
    new_cli->next = NULL;
    // update max_fd
    if (max_fd < fd)
        max_fd = fd;
    // printf("updating max_fd : %d\n", max_fd);
    // link new node to list
    if (last == NULL)
        cli = new_cli;
    else
        last->next = new_cli;
    // add fd to fd_set
    FD_SET(new_cli->fd, &afds);
    printf("| new client %d | fd : %d \n", new_cli->id, new_cli->fd);
    // print_cli("new client");
    return (new_cli->id);
}

int rmv_cli(int fd){
    // remove client
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
    // reset max_fd
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
            printf("| sending to cli %d | fd : %d | msg:%s", tmp->id, tmp->fd, line);
            if (line[strlen(line) - 1] != '\n')
                printf("\n");
            if (send(tmp->fd, line, strlen(line), 0) < 0){
                printf("error send\n");
                ft_err("send_to_others");                
            }

            }
        }
    }
    bzero(buf, buf_size);
}

int main(int ac, char **av) {
	socklen_t           addr_size;
	struct sockaddr_in  serv_addr, cli_addr; 

    // argument control
    if (ac != 2){
        write(2,"Wrong number of arguments\n", strlen("Wrong number of arguments\n"));
        exit(1);
    }
	// socket create and verification 
	sfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sfd == -1)
        ft_err("socket");
    int optval = 1;
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror("setsockopt failed");
        close(sfd);
        return -1;
    }
    if (fcntl(sfd, F_SETFL, O_NONBLOCK) == -1) {
        perror("Failed to set non-blocking mode");
        close(sfd);
        return -1;
    }
	// assign IP, PORT 
	addr_size = sizeof(cli_addr);
    bzero(&serv_addr, addr_size); 
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(atoi(av[1])); 
	serv_addr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	// Binding newly created socket to given IP and verification 
	if ((bind(sfd, (const struct sockaddr *)&serv_addr, addr_size)) != 0)
        ft_err("bind");
	if (listen(sfd, 10) != 0)
        ft_err("listen");
    // initializing fd_sets
    FD_ZERO(&afds);
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_SET(sfd, &afds);
    FD_SET(sfd, &rfds);
    FD_SET(sfd, &wfds);
    max_fd = sfd;
    // server loop
    while (1){
        rfds = wfds = afds;
        int ready = select(max_fd + 1, &rfds, &wfds, NULL, NULL);
        if (ready < 0)
            ft_err("select");
        else if (ready == 0){
            continue;            
        }
        else {
            // new connection
            if (FD_ISSET(sfd, &rfds)){
                int cfd = accept(sfd, (struct sockaddr *)&cli_addr, &addr_size);
                if (cfd < 0)
                    ft_err("accept");
                sprintf(buf, "server: client %d just arrived\n", add_cli(cfd));
                send_to_others(buf, cfd);
            }
            // receive request
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
                            printf("Removing inactive connection | cli_id : %d | fd : %d |\n", tmp->id, tmp->fd);
                            // removing inactive connection
                            t_cli *next = tmp->next;
                            int fd = tmp->fd;
                            // int id = tmp->id;
                            sprintf(buf, "server: client %d just left\n", rmv_cli(tmp->fd));
                            send_to_others(buf, fd);
                            tmp = next;
                        }
                        // respond to request
                        else {
                            printf("Respond to request | cli_id : %d | fd : %d\n| req : %s\n", tmp->id, tmp->fd, buf);
                            int nb_newline = 0;
                            for (int i = 0; buf[i] != 0; i++){
                                if (buf[i] == '\n')
                                    nb_newline++;
                            }
                            printf("nb_newline: %d\n", nb_newline);
                            int     resp_buf_size = buf_size + ((nb_newline + 1) * strlen("client  : "));//!%d 
                            printf("buf_size: %d\n", buf_size);
                            printf("resp_buf_size: %d\n", resp_buf_size);
                            char    send_buf[resp_buf_size];
                            char    tmp_buf[resp_buf_size];
                            bzero(send_buf, resp_buf_size);
                            bzero(tmp_buf, resp_buf_size);
                            
                            sprintf(send_buf, "client %d: ", tmp->id);
                            printf("send_buf: %s\n", send_buf);

                            for (int i = 0; buf[i] != 0; i++){
                                bzero(tmp_buf, resp_buf_size);
                                sprintf(tmp_buf, "%c", buf[i]);
                                strcat(send_buf, tmp_buf);
                                printf("tmp_buf: %s\n", tmp_buf);
                                printf("send_buf: %s\n", send_buf);
                                if (buf[i] == '\n' && buf[i + 1] != 0)
                                {
                                    bzero(tmp_buf, resp_buf_size);
                                    sprintf(tmp_buf, "client %d: ", tmp->id);
                                    strcat(send_buf, tmp_buf);
                                    printf("tmp_buf: %s\n", tmp_buf);
                                    printf("send_buf: %s\n", send_buf);
                                }
                            }
                            printf("\nsend_buf : %s\n", send_buf);
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
