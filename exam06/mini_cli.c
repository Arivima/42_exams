#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>     // select
#include <fcntl.h>          // fcntl

// colors
# define RESET          "\033[0m"
# define BLACK          "\033[30m"
# define RED            "\033[31m"
# define GREEN          "\033[32m"
# define YELLOW         "\033[33m"
# define BLUE           "\033[34m"
# define MAGENTA        "\033[35m"
# define CYAN           "\033[36m"
# define WHITE          "\033[37m"
# define BOLDBLACK      "\033[1m\033[30m"
# define BOLDRED        "\033[1m\033[31m"
# define BOLDGREEN      "\033[1m\033[32m"
# define BOLDYELLOW     "\033[1m\033[33m"
# define BOLDBLUE       "\033[1m\033[34m"
# define BOLDMAGENTA    "\033[1m\033[35m"
# define BOLDCYAN       "\033[1m\033[36m"
# define BOLDWHITE      "\033[1m\033[37m"

// defines
#define NB_CLI          5
#define MAX_MESSAGE_LEN 1024
#define DEFAULT_PORT    8080
#define MINI_SERV_IP    2130706433  /*127.0.0.1*/

// structures
typedef struct  s_cli {
    int fd;
    int id;
}               t_cli;

// global var
int         mini_serv_port = DEFAULT_PORT;
t_cli       clients[NB_CLI];
char        message[MAX_MESSAGE_LEN];
int         max_fd = 0;
fd_set      afds;
fd_set      rfds;
fd_set      wfds;

// functions prototypes
int     start_client(int id);
void    kill_client(int cli_id);
void    update_max_fd();
int     get_user_input();
int     ft_send();
int     ft_inbox();
int     ft_connect();
void    ft_status();
int     ft_kill();
void    ft_exit();

// functions definitions
int start_client(int id)
{
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror(RED "Socket creation failed" RESET);
        return -1;
    }
    int optval = 1;
    if (setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror(RED "setsockopt failed" RESET);
        close(client_fd);
        return -1;
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(MINI_SERV_IP); 
    server_addr.sin_port = htons(mini_serv_port);

    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror(RED "Connection failed" RESET);
        close(client_fd);
        return -1;
    }
    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
        perror(RED "Failed to set non-blocking mode" RESET);
        close(client_fd);
        return -1;
    }
    FD_SET(client_fd, &afds);
    printf(GREEN "Client %d connected to the server\n" RESET, id);
    return client_fd;
}


void    kill_client(int cli_id)
{
    if (clients[cli_id].id != -1){
        printf(CYAN "killing client %d ...\n" RESET, cli_id);
        FD_CLR(clients[cli_id].fd, &afds);
        if (close (clients[cli_id].fd) == -1)
            perror(RED "error close" RESET);
        clients[cli_id].id = -1;
        clients[cli_id].fd = -1;
        update_max_fd();
    }
    else
        printf(RED "client %d already inactive\n" RESET, cli_id);
}


void    ft_exit()
{
    printf(BOLDCYAN "\nExiting...\n" RESET);
    for (int i = 0 ; i < NB_CLI ; i++){
        if (clients[i].id != -1)
            kill_client(i);
    }
    exit(1);    
}


void update_max_fd()
{
    max_fd = 0;
    for (int id = 0 ; id < NB_CLI ; id++){
        if (clients[id].fd > max_fd)
        max_fd = clients[id].fd;
    }
    printf(RED "max_fd = %d\n" RESET, max_fd);
}


void ft_status()
{
    printf(BOLDCYAN "Showing client status\n\n" RESET);
    for (int i = 0 ; i < NB_CLI ; i++){
        if (clients[i].id == -1)
            printf("Client #%d :" RED " inactive\n" RESET, i);
        else
            printf("Client #%d :" GREEN " active\t|fd : %d\n" RESET, i, clients[i].fd);
    }
    printf("\n");
}


int ft_connect()
{
    // connecting clients
    printf(BOLDCYAN "Which client would you like to connect ? (enter : all | or : 0-%d): \n" RESET, NB_CLI - 1);
    if (get_user_input() == -1)
        return -1;
    // checking input
    if (strcmp(message, "all") == 0) {
        for (int id = 0; id < NB_CLI; id++){
            int fd = start_client(id);
            if (fd == -1)
                return -1;
            clients[id].id = id;
            clients[id].fd = fd;
        }            
    }
    else {
        int cli_id = atoi(message);
        if (cli_id < 0 || cli_id >= NB_CLI){
            perror(RED "Invalid input" RESET);
            return -1;
        }
        int fd = start_client(cli_id);
        if (fd == -1)
            return -1;
        clients[cli_id].id = cli_id;
        clients[cli_id].fd = fd;
    }
    update_max_fd();
    return 0;
}

int ft_inbox()
{
    printf(BOLDCYAN "Checking clients' inbox\n" RESET);
    rfds = afds;
    int ready = select(max_fd + 1, &rfds, NULL, NULL, NULL); //! blocked when nothing to read
    printf("ready : %d\n", ready);
    if (ready < 0)
        ft_exit();
    else if (ready == 0){
        printf(RED "No inbox\n" RESET);
        return (-1);            
    }
    else {
        for (int id = 0 ; id < NB_CLI ; id++){
            if (FD_ISSET(clients[id].fd, &rfds))
            {
                bzero(message, MAX_MESSAGE_LEN);
                int bytes_recv = recv(clients[id].fd, message, MAX_MESSAGE_LEN, 0);
                if (bytes_recv < 0)
                    kill_client(id);
                else if (bytes_recv == 0)
                    kill_client(id);
                else
                    printf(GREEN "New inbox : " RESET CYAN "Client #%d" RESET "\n%s\n", id, message);
                bzero(message, MAX_MESSAGE_LEN);
            }
        }
    }
    return 0;
}

int ft_send()
{
    printf(BOLDCYAN "Sending a message\n" RESET);
    wfds = afds;
    int ready = select(max_fd + 1, NULL, &wfds, NULL, NULL);
    printf("ready : %d\n", ready);
    if (ready < 0)
        ft_exit();
    else if (ready == 0){
        printf("Passed select(): No fd ready\n");
        return -1;            
    }
    else {
        // getting client id
        printf(CYAN "which client should send the message: \n" RESET);
        if (get_user_input() == -1)
            return -1;
        // checking input
        int cli_id = atoi(message);
        if (cli_id < 0 || cli_id >= NB_CLI){
            perror(RED "Invalid input" RESET);
            return -1;
        }
        
        printf(CYAN "Enter your message (max %d characters): \n" RESET, MAX_MESSAGE_LEN);
        if (get_user_input() == -1)
            return -1;

        // check if cli_id is active && socket is available for sending data
        if (clients[cli_id].id != -1 && FD_ISSET(clients[cli_id].fd, &wfds)){
            if (send(clients[cli_id].fd, message, strlen(message), 0) < 0) {
                perror(RED "Failed to send message" RESET);
                kill_client(cli_id);
                return -1;
            }
            printf(GREEN "Client #%d successfully sent message :\n%s\n" RESET, cli_id, message);
        }
        else
            printf(RED "Client inactive or socket unavailable" RESET);            
        bzero(message, MAX_MESSAGE_LEN);
    }
    return 0;
}

int get_user_input()
{
    bzero(message, MAX_MESSAGE_LEN);
    printf(MAGENTA "\n>>");
    if (fgets(message, MAX_MESSAGE_LEN, stdin) == NULL) {
        perror(RED "Failed to read input" RESET);
        return -1;
    }
    printf(RESET "\n"); 
    message[strlen(message) - 1] = '\0';
    if (!message){
        perror(RED "Empty input" RESET);
        return -1;
    }
    return 0;
}

int ft_kill()
{
    // getting client id
    printf(BOLDCYAN "which client should be killed ? 0-%d: \n" RESET, NB_CLI - 1);
    if (get_user_input() == -1)
        return -1;
    // checking input
    int cli_id = atoi(message);
    if (cli_id < 0 || cli_id >= NB_CLI){
        perror(RED "Invalid input" RESET);
        return -1;
    }
    kill_client(cli_id);
    return -1;
}

int main(int ac, char **av)
{
    // checking argument validity
    if (ac > 2){
        fprintf(stderr, RED "Usage: %s <mini_serv_port>\n" RESET, av[0]);
        exit(1);
    }
    if (ac == 2){
        mini_serv_port = atoi(av[1]);
        if (mini_serv_port != 0 && (mini_serv_port < 1024 || mini_serv_port > 49151)) {
            fprintf(stderr, RED "Usage: %s <mini_serv_port : 1024 to 49151, default 8080>\n" RESET, av[0]);
            exit(1);
        }        
    }

    // initialize client struct
    bzero(clients, sizeof(t_cli));
    for (int i = 0 ; i < NB_CLI ; i++){
        clients[i].id = -1;
        clients[i].fd = -1;
    }

    // waiting user to be ready
    while (1){
        printf( BOLDCYAN "Welcome to mini_cli, a tester for exam06 : mini_serv\n" RESET);
        printf("\nAre you ready to connect %d clients ?\n", NB_CLI);
        printf("1.Check the port is same as mini_serv" CYAN "\n ->current port : %d\n" RESET, mini_serv_port);
        printf("2.If needed, change number of clients in \"#define NB_CLI\"" CYAN "\n ->current #clients : %d\n" RESET, NB_CLI);
        printf("3.Launch ./mini_serv\n");
        printf(BOLDCYAN"\nWhen you are ready, enter \"" RESET GREEN "ok" RESET "\":\n" RESET);

        if (get_user_input() == -1)
            return -1;
        if (strcmp(message, "ok") == 0)
            break;
        else
            continue;
    }

    // client generator loop
    FD_ZERO(&afds);
    while (1) {
        // asking command
        printf(BOLDCYAN "\nUse one of the following commands :\n" RESET);
        printf(CYAN "<status>\t" RESET ": get a status of connections\n");
        printf(CYAN "<connect>\t" RESET ": kill a specific client\n");
        printf(CYAN "<inbox>\t\t" RESET ": check all clients' inbox\n");
        printf(CYAN "<send>\t\t" RESET ": send a message to server from a  specific client\n");
        printf(CYAN "<kill>\t\t" RESET ": kill a specific connection\n");
        printf(CYAN "<exit>\t\t" RESET ": exit the program\n");

        // getting command from user
        if (get_user_input() == -1)
            continue;

        // processing command
        if      (strcmp(message, "status") == 0)
            ft_status();
        else if (strcmp(message, "connect") == 0)
            ft_connect();
        else if (strcmp(message, "inbox") == 0 && max_fd > 2)
            ft_inbox();
        else if (strcmp(message, "send") == 0 && max_fd > 2)
            ft_send();
        else if (strcmp(message, "kill") == 0 && max_fd > 2)
            ft_kill();
        else if (strcmp(message, "exit") == 0)
            ft_exit();
        else 
            printf(RED "Invalid or empty input, or command required active connection : %s\n" RESET, message);
    }
    return (0);
}