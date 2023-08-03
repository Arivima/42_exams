// includes
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
#define MAX_NB_OPEN_FD      1024                /*max nb of open fd on linux*/
#define NB_CLI              MAX_NB_OPEN_FD
#define MAX_MESSAGE_LEN     1024
#define DEFAULT_PORT        8080
#define MINI_SERV_IP        2130706433          /*127.0.0.1*/
#define ERR                 (-1)
#define PORT_RESERVED_MIN   1024
#define PORT_RESERVED_MAX   49151

// structures
typedef struct  s_cli {
    int id;
}               t_cli;

// global var
int         mini_serv_port = DEFAULT_PORT;
t_cli       cli[NB_CLI];
char        message[MAX_MESSAGE_LEN];
int         max_fd = 0;
int         active_cli = 0;
int         next_id = 0;
fd_set      afds, rfds, wfds;

// functions prototypes
int     start_client(void);
int     get_cli_fd(int id);
int     kill_client(int fd);
void    update_max_fd();
int     get_user_input();
int     ft_send();
int     ft_inbox();
int     ft_connect();
void    ft_status();
int     ft_kill();
void    ft_exit();

// functions definitions
int start_client(void)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror(RED "Socket creation failed" RESET);
        return ERR;
    }

    int optval = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == ERR) {
        perror(RED "setsockopt failed" RESET);
        close(fd);
        return ERR;
    }

    struct sockaddr_in cli_addr;
    memset(&cli_addr, 0, sizeof(cli_addr));
    cli_addr.sin_family = AF_INET;
	cli_addr.sin_addr.s_addr = htonl(MINI_SERV_IP); 
    cli_addr.sin_port = htons(mini_serv_port);

    if (connect(fd, (struct sockaddr*)&cli_addr, sizeof(cli_addr)) < 0) {
        perror(RED "Connection failed" RESET);
        close(fd);
        return ERR;
    }
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == ERR) {
        perror(RED "Failed to set non-blocking mode" RESET);
        close(fd);
        return ERR;
    }

    cli[fd].id = next_id++;
    active_cli++;
    max_fd = max_fd < fd ? fd : max_fd;
    FD_SET(fd, &afds);

    printf(GREEN "Client %d connected to the server using fd %d\n" RESET, cli[fd].id, fd);
    return fd;
}


int get_cli_fd(int id)
{
    for (int fdi = 0; fdi <= max_fd ; fdi++){
        if (cli[fdi].id == id)
            return fdi;
    }
    return (ERR) ;
}

int    kill_client(int fd)
{
    if (cli[fd].id >= 0){
        printf(CYAN "killing client %d ...\n" RESET, cli[fd].id);
        FD_CLR(fd, &afds);
        if (close(fd) == ERR)
            perror(RED "error close" RESET);
        cli[fd].id = -1;
        active_cli--;
        update_max_fd();
        return (0);
    }
    return (ERR);
}


void    ft_exit()
{
    printf(BOLDCYAN "\nExiting...\n" RESET);
    for (int fd = 0 ; fd <= max_fd ; fd++){
        if (cli[fd].id >= 0)
            kill_client(fd);
        sleep(1); // otherwise crashes
    }
    exit(1);    
}


void update_max_fd()
{
    max_fd = 0;
    for (int fdi = 0 ; fdi < NB_CLI ; fdi++)
        if (cli[fdi].id >= 0)
            max_fd = max_fd < fdi ? fdi : max_fd;
    printf("updated max_fd = %d\n", max_fd);
}


void ft_status()
{
    printf(BOLDCYAN "Showing client status\n\n" RESET);
    printf("Number of active clients : %d\n", active_cli);
    for (int fdi = 0 ; fdi <= max_fd ; fdi++){
        if (cli[fdi].id != -1)
            printf("Client #%d :" GREEN " active\t|fd : %d\n" RESET, cli[fdi].id, fdi);
    }
    printf("\n");
}


int ft_connect()
{
    // connecting cli
    printf(BOLDCYAN "How many cli would you like to connect ? (enter : 1-%d ): \n" RESET, NB_CLI);
    if (get_user_input() == ERR)
        return ERR;
    // checking input
    int nb_new_cli = atoi(message);
    if (nb_new_cli < 1 || nb_new_cli + active_cli > NB_CLI){
        printf(RED "Invalid input | cli already active : %d\n" RESET, active_cli);
        return ERR;
    }
    for (int i = 0; i < nb_new_cli; i++)
        if (start_client() == ERR)
            return ERR;
    return 0;
}

int ft_inbox()
{
    if (active_cli == 0){
        printf(RED "Command requiring active clients\n" RESET );
        return ERR;
    }
    printf(BOLDCYAN "Checking cli' inbox\n" RESET);
    rfds = afds;
    int ready = select(max_fd + 1, &rfds, NULL, NULL, NULL); //! blocked when nothing to read
    printf("ready : %d\n", ready);
    if (ready < 0)
        ft_exit();
    else if (ready == 0){
        printf(RED "No inbox\n" RESET);
        return (ERR);            
    }
    else {
        for (int fdi = 0 ; fdi <= max_fd ; fdi++){
            if (FD_ISSET(fdi, &rfds))
            {
                int id = cli[fdi].id;
                bzero(message, MAX_MESSAGE_LEN);
                int bytes_recv = recv(fdi, message, MAX_MESSAGE_LEN, 0);
                if (bytes_recv <= 0){
                    kill_client(fdi);
                }
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
    if (active_cli == 0){
        printf(RED "Command requiring active clients\n" RESET );
        return ERR;
    }

    printf(BOLDCYAN "Sending a message\n" RESET);
    wfds = afds;
    int ready = select(max_fd + 1, NULL, &wfds, NULL, NULL);
    printf("ready : %d\n", ready);
    if (ready < 0)
        ft_exit();
    else if (ready == 0){
        printf("Passed select(): No fd ready\n");
        return ERR;            
    }
    else {
        int fd = -1;
        int cli_id = -1;
        // get client id
        while (1){
            printf(CYAN "which client should send the message: \n" RESET);
            if (get_user_input() == ERR)
                continue;
            cli_id = atoi(message);
            if (cli_id < 0 || cli_id >= NB_CLI) {
                perror(RED "Invalid input" RESET);
                continue;
            }
            if ((fd = get_cli_fd(cli_id)) == ERR) {
                perror(RED "Client inactive" RESET);
                continue;
            }
            break;
        }
        // get message
        while (1) {
            printf(CYAN "Enter your message (max %d characters): \n" RESET, MAX_MESSAGE_LEN - 1);
            if (get_user_input() == ERR)
                continue;
            if (strlen(message) >= MAX_MESSAGE_LEN){
                printf(RED "Message too long\n" RESET);
                continue;
            }
            break;
        }
        // send message
        if (FD_ISSET(fd, &wfds)){
            if (send(fd, message, strlen(message), 0) < 0) {
                perror(RED "Failed to send message" RESET);
                kill_client(fd);
                return ERR;
            }
            printf(GREEN "Client #%d successfully sent message :\n%s\n" RESET, cli_id, message);
        }
        else
            printf(RED "Socket unavailable" RESET);            
        bzero(message, MAX_MESSAGE_LEN);
    }
    return 0;
}

int get_user_input()
{
    bzero(message, MAX_MESSAGE_LEN);
    printf(MAGENTA "\n>> ");
    if (fgets(message, MAX_MESSAGE_LEN, stdin) == NULL) {
        perror(RED "Failed to read input" RESET);
        return ERR;
    }
    printf(RESET "\n"); 
    message[strlen(message) - 1] = '\0';
    if (!message || strcmp(message, "") == 0) {
        perror(RED "Empty input" RESET);
        return ERR;
    }
    if (strcmp(message, "main") == 0){
        printf(YELLOW "Returning to main\n" RESET);
        return ERR;
    }
    return 0;
}

int ft_kill()
{
    if (active_cli == 0){
        printf(RED "Command requiring active clients\n" RESET );
        return ERR;
    }

    // getting client id
    printf(BOLDCYAN "which client should be killed ? enter a client id\n" RESET);
    ft_status();
    if (get_user_input() == ERR)
        return ERR;
    // checking input
    int cli_id = atoi(message);
    if (cli_id < 0 || cli_id >= NB_CLI){
        perror(RED "Invalid input" RESET);
        return ERR;
    }
    int fd = get_cli_fd(cli_id);
    if (fd != ERR)
        kill_client(fd);
    return ERR;
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
        if (mini_serv_port != 0 && (mini_serv_port < PORT_RESERVED_MIN || mini_serv_port > PORT_RESERVED_MAX)) {
            fprintf(stderr, RED "Usage: %s <mini_serv_port : 1024 to 49151, default 8080>\n" RESET, av[0]);
            exit(1);
        }        
    }

    // initialize client struct
    bzero(cli, sizeof(t_cli));
    for (int i = 0 ; i < NB_CLI ; i++){
        cli[i].id = -1;
    }

    // waiting user to be ready
    while (1){
        printf( BOLDCYAN "Welcome to mini_cli, a tester for exam06 mini_serv\n\n" RESET);
        printf("default values\n");
        printf("| ip\t\t127.0.0.1\n");
        printf("| port\t\t8080\n");
        printf("| max nb of cli\t1024\n\n");
        printf("1.Launch ./mini_serv\n");
        printf("2.Check the port is same as for mini_serv" CYAN "\n ->current port : %d\n" RESET, mini_serv_port);
        printf(BOLDCYAN"\nWhen you are ready to connect clients, enter \"" RESET GREEN "ok" RESET BOLDCYAN"\":\n" RESET);

        if (get_user_input() == ERR)
            return ERR;
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
        printf(CYAN "<inbox>\t\t" RESET ": check all cli' inbox\n");
        printf(CYAN "<send>\t\t" RESET ": send a message to server from a  specific client\n");
        printf(CYAN "<kill>\t\t" RESET ": kill a specific connection\n");
        printf(CYAN "<exit>\t\t" RESET ": exit the program\n\n");
        printf(YELLOW "You can go back to the main menu by inputing \"main\"\n" RESET);

        // getting command from user
        if (get_user_input() == ERR)
            continue;

        // processing command
        if      (strcmp(message, "status") == 0)
            ft_status();
        else if (strcmp(message, "connect") == 0)
            ft_connect();
        else if (strcmp(message, "inbox") == 0)
            ft_inbox();
        else if (strcmp(message, "send") == 0)
            ft_send();
        else if (strcmp(message, "kill") == 0)
            ft_kill();
        else if (strcmp(message, "exit") == 0)
            ft_exit();
        else 
            printf(RED "Invalid or empty input : %s\n" RESET, message);
    }
    return (0);
}