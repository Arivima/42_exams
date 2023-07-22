// #include <errno.h>
#include <stdio.h>     // DEBUG
#include <stdlib.h>     // exit
#include <string.h>     // strlen
#include <unistd.h>     // write, NULL, close, exit
// #include <netdb.h>
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
void    print_cli(const char * title);
int     add_cli(int fd);
int     rmv_cli(int fd);
void     send_to_others(char *line, int fd);


void    ft_err()
{
    printf("ft_err()\n");
    for (t_cli *tmp = cli; tmp != NULL; tmp = tmp->next)
        rmv_cli(tmp->fd);
    if (sfd > 0)
        close(sfd);
    write(2, "Fatal error\n", strlen("Fatal error\n")); 
    exit(1);     
}

void    print_cli(const char * title){
    printf("| print_cli() | %s\n", title);
    for (t_cli *tmp = cli; tmp != NULL; tmp = tmp->next){
        printf("| client | id : %d | fd : %d | next %p\n", tmp->id, tmp->fd, tmp->next);
    }
    if (cli == NULL)
    printf("| no clients\n");
}

int    add_cli(int fd)
{
    printf("add_cli()\n");
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
        ft_err();
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
    print_cli("after adding");
    return (new_cli->id);
}

int rmv_cli(int fd){
    printf("rmv_cli()\n");
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
    print_cli("after removing");
    // reset max_fd
    max_fd = 0;
    for (t_cli *tmp = cli; tmp != NULL; tmp = tmp->next){
        if (max_fd < tmp->fd)
            max_fd = tmp->fd;
    }
    printf("resetting max_fd : %d\n", max_fd);
    return (id);
}

void send_to_others(char *line, int fd)
{
    printf("send_to_others()\n");
    for (t_cli *tmp = cli; tmp != NULL; tmp = tmp->next){
        if (FD_ISSET(tmp->fd, &wfds))
        {
            if (tmp->fd != fd){
            printf("| send to cli | id : %d | fd : %d | msg: %s |\n", tmp->id, tmp->fd, line);
            if (send(tmp->fd, line, strlen(line), 0) < 0)
                ft_err();
            }
            else
                printf("| client will not send to itself | id : %d | fd : %d |\n", tmp->id, tmp->fd);
        }
        else
            printf("| cli not ready for writing | id : %d | fd : %d |\n", tmp->id, tmp->fd);
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
	//! sfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); 
	sfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); 
	if (sfd == -1)
        ft_err();
	// assign IP, PORT 
	addr_size = sizeof(cli_addr);
    bzero(&serv_addr, addr_size); 
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(atoi(av[1])); 
	serv_addr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	// Binding newly created socket to given IP and verification 
	if ((bind(sfd, (const struct sockaddr *)&serv_addr, addr_size)) != 0)
        ft_err();
	if (listen(sfd, 10) != 0)
        ft_err();
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
        printf("Server waiting...\n");
        int ready = select(max_fd + 1, &rfds, &wfds, NULL, NULL);
        printf("ready : %d\n", ready);
        if (ready < 0)
            ft_err();
        else if (ready == 0){
            printf("Passed select(): No fd ready\n");
            continue;            
        }

        else {
            if (FD_ISSET(sfd, &rfds)){
                printf("Accepting new connection\n");
                // new connection
                int cfd = accept(sfd, (struct sockaddr *)&cli_addr, &addr_size);
                if (cfd < 0)
                    ft_err();
                sprintf(buf, "server: client %d just arrived\n", add_cli(cfd));
                printf("buffer : %s\n", buf);
                send_to_others(buf, cfd);
            }
            for (t_cli *tmp = cli; tmp != NULL; tmp = tmp->next){
                if (FD_ISSET(tmp->fd, &rfds)){
                    printf("Receiving a request | cli_id : %d | fd : %d |\n", tmp->id, tmp->fd);
                    // receive request
                    ssize_t bytes_recv = recv(tmp->fd, &buf, buf_size, 0);
                    if (bytes_recv == -1)
                        ft_err(); //! exit ? && if not buf
                    else if (bytes_recv == 0)
                    {
                        printf("Removing inactive connection | cli_id : %d | fd : %d |\n", tmp->id, tmp->fd);
                        // removing inactive connection
                        sprintf(buf, "server: client %d just left\n", rmv_cli(tmp->fd));
                        printf("buffer : %s\n", buf);
                        send_to_others(buf, tmp->fd);
                    }
                    else {
                        printf("Respond to request | cli_id : %d | fd : %d\n| req : %s\n", tmp->id, tmp->fd, buf);
                        // respond to request
                        int nb_newline = 0;
                        for (int i = 0; buf[i] != 0; i++){
                            if (buf[i] == '\n')
                                nb_newline++;
                        }
                        printf("nb_newline: %d\n", nb_newline);
                        int     resp_buf_size = buf_size + (nb_newline * strlen("client %d: "));
                        printf("resp_buf_size: %d\n", resp_buf_size);
                        char    send_buf[resp_buf_size];
                        bzero(send_buf, resp_buf_size);
                        sprintf(send_buf, "client %d: ", tmp->id);
                        for (int i = 0; buf[i] != 0; i++){
                            sprintf(send_buf, "%c", buf[i]);
                            if (buf[i] == '\n' && buf[i + 1] != 0)
                                sprintf(send_buf, "client %d: ", tmp->id);
                        }
                        printf("buffer : %s\n", send_buf);
                        send_to_others(send_buf, tmp->fd);
                    }
                }
            }
        }
    }
}
