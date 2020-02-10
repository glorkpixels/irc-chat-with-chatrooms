#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#define SOCKETERROR (-1)
#define USERNAMELENGHT 20
#define RECEIVEINGMESGLENGHT 111
#define SENDINGMESGLENGHT 222
#define SERVERPORT 3205
///defines to use
// library includes


//globalvariables
volatile sig_atomic_t flag = 0;
int sockfd = 0;
char nickname[USERNAMELENGHT] = {};

void exit(int sig) {
    flag = 1;
}
void str_trim_lf (char* arr, int length) {
    int i;
    for (i = 0; i < length; i++) { // trim \n
        if (arr[i] == '\n') {
            arr[i] = '\0';
            break;
        }
    }
}
void str_overwrite_stdout() {
    printf("\r%s", "> ");
    fflush(stdout);
}
void recv_msg_handler() { //receiveMSG default
    char receiveMessage[SENDINGMESGLENGHT] = {};
    while (1) {
        int receive = recv(sockfd, receiveMessage, SENDINGMESGLENGHT, 0);
        if (receive > 0) {
            printf("\r%s\n", receiveMessage);
            str_overwrite_stdout();
        } else if (receive == 0) {
            break;
        } else { 
            // -1 
        }
    }
}

void send_msg_handler() {//sentMSG default
    char message[RECEIVEINGMESGLENGHT] = {};
    while (1) {
        str_overwrite_stdout();
        while (fgets(message, RECEIVEINGMESGLENGHT, stdin) != NULL) {
            str_trim_lf(message, RECEIVEINGMESGLENGHT);
            if (strlen(message) == 0) {
                str_overwrite_stdout();
            } else {
                break;
            }
        }
        send(sockfd, message, RECEIVEINGMESGLENGHT, 0);
        if (strcmp(message, "exit") == 0) {
            break;
        }
    }
    exit(2);
}

int main()
{
    signal(SIGINT, exit);

    // Naming on the server 
    printf("Please enter your name: ");
    if (fgets(nickname, USERNAMELENGHT, stdin) != NULL) {
        str_trim_lf(nickname, USERNAMELENGHT);
    }
    if (strlen(nickname) < 2 || strlen(nickname) >= USERNAMELENGHT-1) {
        printf("\nName must be SHORTER THAN thirty characters.\n");
        exit(EXIT_FAILURE);
    }

    // Create socket user socket
    sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if (sockfd == -1) {
        printf("Fail to create a socket.");
        exit(EXIT_FAILURE);
    }

    // Socket information of user
    struct sockaddr_in server_info, client_info;
    int s_addrlen = sizeof(server_info);
    int c_addrlen = sizeof(client_info);
    memset(&server_info, 0, s_addrlen);
    memset(&client_info, 0, c_addrlen);
    server_info.sin_family = PF_INET;
    server_info.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_info.sin_port = htons(SERVERPORT);

    // Connect to Server
    int err = connect(sockfd, (struct sockaddr *)&server_info, s_addrlen);
    if (err == -1) {
        printf("Connection to Server error!\n");
        exit(EXIT_FAILURE);
    }
    
    //getting socket name and peer name to send to the server
    getsockname(sockfd, (struct sockaddr*) &client_info, (socklen_t*) &c_addrlen);
    getpeername(sockfd, (struct sockaddr*) &server_info, (socklen_t*) &s_addrlen);
    printf("Connected to the server of this: %s:%d\n", inet_ntoa(server_info.sin_addr), ntohs(server_info.sin_port));
    printf("You are this: %s:%d\n", inet_ntoa(client_info.sin_addr), ntohs(client_info.sin_port));

    send(sockfd, nickname, USERNAMELENGHT, 0);

    pthread_t send_msg_thread;
    if (pthread_create(&send_msg_thread, NULL, (void *) send_msg_handler, NULL) != 0) {
        printf ("Create pthread error!\n");
        exit(EXIT_FAILURE);
    }

    pthread_t recv_msg_thread;
    if (pthread_create(&recv_msg_thread, NULL, (void *) recv_msg_handler, NULL) != 0) {
        printf ("Create pthread error!\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        if(flag) {
            printf("\n WE ARE SORRY WE CAN'T WORK PROPERLY X(    \n");
            break;
        }
    }

    close(sockfd);
    return 0;
}