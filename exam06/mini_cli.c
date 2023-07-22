#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h> // select

#define MAX_MESSAGE_LEN 1024
#define MINI_SERV_PORT  8080
#define MINI_SERV_IP    2130706433  /*127.0.0.1*/


// predefined port and IP
// prog arg : <nb_of_clients>
// commands : send a msg to server from client #id :  <client_id> <message>
// commands : kill a connection : <client_id> <"kill">
// commands : exit the program : <client_id> <"exit">

// structures
typedef struct  s_cli {
    int fd;
    int id;
    s_cli * next;
}               t_cli;

// global var
char message[MAX_MESSAGE_LEN];
fd_set      afds, rfds, wfds;

// functions
int     start_client(int id);
void    ft_kill(t_cli ** clients, int cli_id);
void    ft_exit(t_cli ** clients, int nb_cli);

int start_client(int id)
{

    int client_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (client_fd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(MINI_SERV_IP); 
    server_addr.sin_port = htons(MINI_SERV_PORT);

    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_fd);
        return 1;
    }

    FD_SET(client_fd, &afds);
    printf("Client %d connected to the server\n", id);
    return client_fd;
}

void    ft_kill(t_cli ** clients, int cli_id)
{
    if ((*clients)[cli_id].id != -1){
        printf("killing client %d ...\n", cli_id);
        FD_CLR((*clients)[cli_id].fd, &afds);
        close ((*clients)[cli_id].fd);
        (*clients)[cli_id].id = -1;
        (*clients)[cli_id].fd = -1;
    }
    else
        printf("client %d already inactive\n", cli_id);
}


void    ft_exit(t_cli ** clients, int nb_cli)
{
    printf("\nExiting...\n");
    for (int i = 0 ; i < nb_cli ; i++){
        if ((*clients)[i].id != -1)
            ft_kill(clients, i);
    }
    exit(1);    
}

int main(int ac, char **av)
{
    // checking argument validity
    if (ac != 2){
        fprintf(stderr, "Usage: %s <nb_of_clients : 1-100>\n", av[0]);
        exit(1);
    }
    int nb_cli = atoi(av[1]);
    if ( nb_cli < 1 || nb_cli > 100){
        fprintf(stderr, "Usage: %s <nb_of_clients : 1-100>\n", av[0]);
        exit(1);
    }

    t_cli   clients[nb_cli];
    bzero(clients, ((nb_cli + 1) * sizeof(t_cli)));
    for (int i = 0 ; i < nb_cli ; i++){
        clients[i].id = -1;
        clients[i].fd = -1;
    }

    // waiting user to set_up clients
    while (1){
        printf("\nAre you ready to connect %d clients ?\n1.Check the port is accurate\n2.Launch ./mini_serv\nEnter \"ok\":\n\n", nb_cli);
        bzero(message, MAX_MESSAGE_LEN);
        if (fgets(message, MAX_MESSAGE_LEN, stdin) == NULL) {
            perror("Failed to read input");
            continue;
        }
        printf("\n");
        message[strcspn(message, "\n")] = '\0';
        if (strcmp(message, "ok") == 0)
            break;
        else
            continue;
    }
     

    // client loop
    FD_ZERO(&afds);
    while (1) {
        // asking command
        printf("\nUse one of the following commands :\n");
        printf("<status>\t: get a status of connections\n");
        printf("<connect>\t: kill a specific client\n");
        printf("<inbox>\t\t: check all clients' inbox\n");
        printf("<send>\t\t: send a message to server from a  specific client\n");
        printf("<kill>\t\t: kill a specific connection\n");
        printf("<exit>\t\t: exit the program\n\n");

        bzero(message, MAX_MESSAGE_LEN);
        if (fgets(message, MAX_MESSAGE_LEN, stdin) == NULL) {
            perror("Failed to read input");
            continue;
        }
        printf("\n");
        // checking input
        message[strcspn(message, "\n")] = '\0';
        if (strcmp(message, "status") == 0){
            for (int i = 0 ; i < nb_cli ; i++){
                if (clients[i].id == -1)
                    printf("Client #%d : inactive\n", i);
                else
                    printf("Client #%d : active\t|fd : %d\n", i, clients[i].fd);
            }
        }
        else if (strcmp(message, "connect") == 0)
        {
            // connecting clients
            printf("\nWhich client would you like to connect ? (enter : all | or : 0-%d): \n\n", nb_cli - 1);
            bzero(message, MAX_MESSAGE_LEN);
            if (fgets(message, MAX_MESSAGE_LEN, stdin) == NULL) {
                perror("Failed to read input");
                continue;
            }
            printf("\n");
            // checking input
            message[strcspn(message, "\n")] = '\0';
            if (strcmp(message, "all") == 0) {
                for (int id = 0; id < nb_cli; id++){
                    clients[id].id = id;
                    clients[id].fd = start_client(id);
                }            
            }
            else {
                int cli_id = atoi(message);
                if (cli_id < 0 || cli_id >= nb_cli){
                    perror("Invalid input");
                    continue;
                }
                clients[cli_id].id = cli_id;
                clients[cli_id].fd = start_client(cli_id);
            }
        }
        else if (strcmp(message, "inbox") == 0) {
            printf("\nChecking clients' inbox\n");
            rfds = afds;
            int ready = select(nb_cli + 1, &rfds, NULL, NULL, NULL);
            printf("ready : %d\n", ready);
            if (ready < 0)
                ft_exit((t_cli**)&clients, nb_cli);
            else if (ready == 0){
                printf("No inbox\n");
                continue;            
            }
            else {
                for (int id = 0 ; id < nb_cli ; id++){
                    if (FD_ISSET(clients[id].fd, &rfds))
                    {
                        bzero(message, MAX_MESSAGE_LEN);
                        int bytes_recv = recv(clients[id].fd, message, MAX_MESSAGE_LEN, 0);
                        if (bytes_recv < 0)
                            ft_kill((t_cli**)&clients, id);
                        else if (bytes_recv == 0)
                            ft_kill((t_cli**)&clients, id);
                        else
                            printf("New inbox : Client #%d\n%s\n", id, message);

                    }
                }
            }

        }
        else if (strcmp(message, "send") == 0) {
            printf("\nSending a message\n");
            wfds = afds;
            int ready = select(nb_cli + 1, NULL, &wfds, NULL, NULL);
            printf("ready : %d\n", ready);
            if (ready < 0)
                ft_exit((t_cli**)&clients, nb_cli);
            else if (ready == 0){
                printf("Passed select(): No fd ready\n");
                continue;            
            }
            else {
                // getting client id
                printf("which client should send the message: \n\n");
                bzero(message, MAX_MESSAGE_LEN);
                if (fgets(message, MAX_MESSAGE_LEN, stdin) == NULL) {
                    perror("Failed to read input");
                    continue;
                }
                printf("\n");
                // checking input
                message[strcspn(message, "\n")] = '\0';
                int cli_id = atoi(message);
                if (cli_id < 0 || cli_id >= nb_cli){
                    perror("Invalid input");
                    continue;
                }
                
                printf("Enter your message (max %d characters): \n\n", MAX_MESSAGE_LEN);
                bzero(message, MAX_MESSAGE_LEN);
                if (fgets(message, MAX_MESSAGE_LEN, stdin) == NULL) {
                    perror("Failed to read input");
                    continue;
                }
                printf("\n");
                message[strcspn(message, "\n")] = '\0';


                // check if cli_id is active && socket is available for sending data
                if (clients[cli_id].id != -1 && FD_ISSET(clients[cli_id].fd, &wfds)){
                    if (send(clients[cli_id].fd, message, strlen(message), 0) < 0) {
                        perror("Failed to send message");
                        ft_kill((t_cli**)&clients, cli_id);
                        continue;
                    }
                    printf("Client #%d sent message :\n%s\n", cli_id, message);
                }
                else
                    printf("Client inactive or socket unavailable");            
            }

        }
        else if (strcmp(message, "kill") == 0) {
            // getting client id
            printf("\nwhich client should be killed ? 0-%d: \n\n", nb_cli - 1);
            bzero(message, MAX_MESSAGE_LEN);
            if (fgets(message, MAX_MESSAGE_LEN, stdin) == NULL) {
                perror("Failed to read input");
                continue;
            }
            printf("\n");
            // checking input
            message[strcspn(message, "\n")] = '\0';
            int cli_id = atoi(message);
            if (cli_id < 0 || cli_id >= nb_cli){
                perror("Invalid input");
                continue;
            }
            ft_kill((t_cli**)&clients, cli_id);
            continue;
        }
        else if (strcmp(message, "exit") == 0) {
            ft_exit((t_cli**)&clients, nb_cli);
        }
        else {
            printf("Invalid or empty input");
            continue;
        }
        
        
        
        
    }
    return (0);
}