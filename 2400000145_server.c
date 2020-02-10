
/*
server is up and running because of  my rookieness on C.
all the structures are well thought 'in my opinion'
my code needed to checked manually.
my server and clients are compiling however I couldn't manage to receive starting parameters from user I think
which is caused by my client_handler() function. If I had manage to do that it would run smoothly 

*/


#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <semaphore.h> // library includes
#define roomcapacity 4
#define chatroom_count 10
#define SERVERPORT 3205
#define SOCKETERROR (-1)
#define USERNAMELENGHT 20
#define RECEIVEINGMESGLENGHT 111
#define SENDINGMESGLENGHT 222 //defines to use

int totalconnnectedclient =0;
int createdroomcount =0;
int maxconnectedclient = 40;


// Global variables
int server_sockfd = 0, client_sockfd = 0;


int connectedusercount = 0;
typedef struct ClientNode {//client node which hold socket id as data ip address and nick name
    int data;
    char ip[16];
    char name[31];
} ClientList;
ClientList *newNode(int sockfd, char* ip) {
    ClientList *np = (ClientList *)malloc( sizeof(ClientList) );
    np->data = sockfd;
    strncpy(np->ip, ip, 16);
    strncpy(np->name, "NULL", 5);
    return np; // client list which all clients gets recorded when connecting
}




struct rooms // rooms get maximum 4 clients and they are in kind of client list so I can add them to the room to talk to each other.
{
    int roomid;
    ClientList *person1;
    ClientList *person2;
	ClientList *person3;
	ClientList *person4;
};
struct rooms room[chatroom_count]; // creating 10 rooms which is defined



void clientdisplay(void *p_client) // client display test if they are noded correctly
{ClientList *np = (ClientList *)p_client;
        printf("username:%s IPadress(%s)(%d)", np->name, np->ip, np->data);
    
    printf("\n");
}

int check (int exp, const char *msg){ //socket error check function to reuse when needed
if(exp = SOCKETERROR)
perror(msg);
exit(1);


return exp;

}


//static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;



void list(void *p_client){ // room lister 
	char send_buffer[SENDINGMESGLENGHT] = {};
	ClientList *np = (ClientList *)p_client;
	int k;
	for (k = 0; k < chatroom_count; k++)
    {
		if(room[k].roomid != NULL){
		sprintf(send_buffer,"There is a room %x",room[k].roomid);
		send_to_one_client(np,send_buffer);
		}
    }
}

void lobby(void *p_client){ //sending lobby text to client upon connection
	 char send_buffer[SENDINGMESGLENGHT] = {};
	
	ClientList *np = (ClientList *)p_client;
	sprintf(send_buffer,"1.)List All Rooms \n");
			send_to_one_client(np,send_buffer);
	sprintf(send_buffer,"2.)Create a room (without name or password) \n");
			send_to_one_client(np,send_buffer);
	sprintf(send_buffer,"3.)Enter a room from listed rooms \n");
			send_to_one_client(np,send_buffer);
	sprintf(send_buffer,"4.)quit to the main lobby (when in a room) \n");
			send_to_one_client(np,send_buffer);
	sprintf(send_buffer,"5.)WHO AM I?\n");
			send_to_one_client(np,send_buffer);
	sprintf(send_buffer,"6.)Quit\n");
			send_to_one_client(np,send_buffer);
}



void enterRoom(int roomnumber,void *p_client){
	int roomcountxd;
	int leave_flag = 0;
    char nickname[USERNAMELENGHT] = {};
    char recv_buffer[RECEIVEINGMESGLENGHT] = {};
    char send_buffer[SENDINGMESGLENGHT] = {};
    ClientList *np = (ClientList *)p_client;

    // Naming
    if (recv(np->data, nickname, USERNAMELENGHT, 0) <= 0 || strlen(nickname) < 2 || strlen(nickname) >= USERNAMELENGHT-1) {
        printf("%s didn't input name.\n", np->ip);
        leave_flag = 1;
    } else {
        strncpy(np->name, nickname, USERNAMELENGHT);
        printf("%s(%s)(%d) join the chatroom.\n", np->name, np->ip, np->data);
        sprintf(send_buffer, "%s(%s) join the chatroom.", np->name, np->ip);
		roomcountxd =1;
        send_to_all_clients(roomcountxd,np, send_buffer);
    }

    // Conversation
    while (1) { // talking in the room establishment 
        if (leave_flag) {
            break;
        }
        int receive = recv(np->data, recv_buffer, RECEIVEINGMESGLENGHT, 0);
        if (receive > 0) {
            if (strlen(recv_buffer) == 0) {
                continue;
            }
            sprintf(send_buffer, "%s：%s from %s", np->name, recv_buffer, np->ip);
        } else if (receive == 0 || strcmp(recv_buffer, "exit") == 0) {
            printf("%s(%s)(%d) leave the chatroom.\n", np->name, np->ip, np->data);
            sprintf(send_buffer, "%s(%s) leave the chatroom.", np->name, np->ip);
            leave_flag = 1;
        } else {
            printf("Fatal Error: -1\n");
            leave_flag = 1;
        }
		roomcountxd =1;
        send_to_all_clients(roomcountxd,np, send_buffer);
    }
	
}
void quitRoom(void *p_client){ //quit room is to exit to maini lobby when doing that we compare and find exiting client and we null its place on room with client list *null pointer
	
	ClientList *np = (ClientList *)p_client;
	ClientList *null = newNode(-1, -1);
	int i;
	for (i = 0; i < chatroom_count; ++i)
	{
		ClientList *ax =(ClientList *)room[i].person1;
        ClientList *bx =(ClientList *)room[i].person2;
		ClientList *cx =(ClientList *)room[i].person3;
		ClientList *dx =(ClientList *)room[i].person4;
	
		if(np->data == ax->data){
		room[i].person1 = null;
		}
		if(np->data == bx->data){
		room[i].person2 = null;
		}
		if(np->data == cx->data){
		room[i].person3 = null;
		}
		if(np->data == dx->data){
		room[i].person4 = null;
		}
	}
	client_handler(np);
}

void createRoom(void *p_client){ // create room which creates room if room count not exceeding limit and redirecting user to enterRoom
	 char send_buffer[SENDINGMESGLENGHT] = {};
		ClientList *np = (ClientList *)p_client;
		if(createdroomcount!=9)
		{
		ClientList *c = newNode(-1, -1);
        room[createdroomcount].roomid = createdroomcount;
        room[createdroomcount].person1 =(ClientList *) np;
        room[createdroomcount].person2 =(ClientList *)c;
		room[createdroomcount].person3 =(ClientList *) c;
		room[createdroomcount].person4 =(ClientList *) c;
		createdroomcount++;
		}
		else{
		
		sprintf(send_buffer,"MAXIMUM ROOM COUNT REACHED CANNOT CREATE ROOM");	
		send_to_one_client(np,send_buffer);
		}
		enterRoom(createdroomcount,(void *)np);

}

void exitProgram(void *p_client){ //exiter with ctrlc
	char send_buffer[SENDINGMESGLENGHT] = {};
	ClientList *np = (ClientList *)p_client;
	sprintf(send_buffer,"OUCH, did you hit Ctrl-C?\n");
	send_to_one_client(np,send_buffer);
	exit(0);
}

void whoamI(void *p_client){ //user credential function
	 char send_buffer[SENDINGMESGLENGHT] = {};
	ClientList *np = (ClientList *)p_client;
	sprintf(send_buffer,"You are %s(%s)(%d) .\n", np->name, np->ip, np->data);
	send_to_one_client(np,send_buffer);
	
}


void send_to_all_clients(int roomid,void *p_client, char tmp_buffer[]) { //sending all the clients in the room when in a room and sending everyone but itself of client
		ClientList *np = (ClientList *)p_client;
		char send_buffer[SENDINGMESGLENGHT] = {};
		ClientList *a =(ClientList *)room[roomid].person1;
        ClientList *b =(ClientList *)room[roomid].person2;
		ClientList *c =(ClientList *)room[roomid].person3;
		ClientList *d =(ClientList *)room[roomid].person4;
	
		if(np->data != a->data){
		printf("Send to sockfd %d: \"%s\" \n", a->data, tmp_buffer);
        send(a->data, tmp_buffer, SENDINGMESGLENGHT, 0);
		}
		if(np->data != b->data){
		printf("Send to sockfd %d: \"%s\" \n", b->data, tmp_buffer);
        send(b->data, tmp_buffer, SENDINGMESGLENGHT, 0);
		}
		if(np->data != c->data){
		printf("Send to sockfd %d: \"%s\" \n", c->data, tmp_buffer);
        send(c->data, tmp_buffer, SENDINGMESGLENGHT, 0);
		}
		if(np->data != d->data){
		printf("Send to sockfd %d: \"%s\" \n", d->data, tmp_buffer);
        send(d->data, tmp_buffer, SENDINGMESGLENGHT, 0);
		}		
	}
 
 void send_to_one_client(ClientList *np, char tmp_buffer[]) { // sending feedback from server to one client
    char send_buffer[SENDINGMESGLENGHT] = {};
	ClientList *xdad = (ClientList *)np;
    
            printf("Send to sockfd %d: \"%s\" \n", xdad->data, tmp_buffer);
            send(xdad->data, tmp_buffer, SENDINGMESGLENGHT, 0);
      
    
}


void client_handler(void *p_client) {
	ClientList *np = (ClientList *)p_client;
    listen(server_sockfd, 5);
	char nickname[USERNAMELENGHT] = {};
    char recv_buffer[RECEIVEINGMESGLENGHT] = {};
    char send_buffer[SENDINGMESGLENGHT] = {};
	lobby(np);
	createRoom(np); // tring the manual creationg
	int number =0;
	int roomselect =-1;
		
	sprintf(send_buffer,"Welcome to the chat server \n");
	send_to_one_client(np,send_buffer);
		int receive = recv(np->data, recv_buffer, RECEIVEINGMESGLENGHT, 0);
       while(1){ //while loop to choose category of case switch
	   if (receive > 0) {
           if (strlen(recv_buffer) == 0) {
                continue;
            }
	   }
		else if (receive == 0 || strcmp(recv_buffer, "1") == 0) {
           number =1;
		   break;
        }
		else if (receive == 0 || strcmp(recv_buffer, "2") == 0) {
           number =2;
		    break;
        }
		else if (receive == 0 || strcmp(recv_buffer, "3") == 0) {
           number =3;
		    break;
        }else if (receive == 0 || strcmp(recv_buffer, "4") == 0) {
           number =4;
		    break;
        }else if (receive == 0 || strcmp(recv_buffer, "5") == 0) {
           number =5;
		    break;
        }else if (receive == 0 || strcmp(recv_buffer, "6") == 0) {
           number =6;
		    break;
        }
		else{
			sprintf(send_buffer,"try again please");
			send_to_one_client(np,send_buffer);
		}
		
	   }
	
	
	
	while(1) // while to go to the functions and sending client infos to communicate back while in there
	{
	switch (number) {
	case 1:
	list(np);
	break;
	case 2:
	createRoom(np);
	break;
	case 3:
	enterRoom(roomselect, np);
    break;
	case 4:
	quitRoom(np);
	break;
	case 5:
	whoamI(np);
	break;
	case 6:
	exitProgram(np);
	break;
	
	default:
    sprintf(send_buffer,"Command not listed on up");
	send_to_one_client(np,send_buffer);
	break;
	}
	break;
	
	}
}

int main()
{

    // Create socket
    server_sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if (server_sockfd == -1) {
        printf("Fail to create a socket.");
        exit(EXIT_FAILURE);
    }

    // Socket information
    struct sockaddr_in server_info, client_info;
    int s_addrlen = sizeof(server_info);
    int c_addrlen = sizeof(client_info);
    memset(&server_info, 0, s_addrlen);
    memset(&client_info, 0, c_addrlen);
    server_info.sin_family = PF_INET;
    server_info.sin_addr.s_addr = INADDR_ANY;
    server_info.sin_port = htons(SERVERPORT);// default connections and server port fix to 3205

    // Bind and Listen
    bind(server_sockfd, (struct sockaddr *)&server_info, s_addrlen);
    listen(server_sockfd, 5);

    // Print Server IP
    getsockname(server_sockfd, (struct sockaddr*) &server_info, (socklen_t*) &s_addrlen);
    printf("Start Server on: %s:%d\n", inet_ntoa(server_info.sin_addr), ntohs(server_info.sin_port));


    while (1) {
        client_sockfd = accept(server_sockfd, (struct sockaddr*) &client_info, (socklen_t*) &c_addrlen);

        // Print Client IP
        getpeername(client_sockfd, (struct sockaddr*) &client_info, (socklen_t*) &c_addrlen);
        printf("Client %s:%d come in.\n", inet_ntoa(client_info.sin_addr), ntohs(client_info.sin_port));

        // Append linked list for clients
        ClientList *c = newNode(client_sockfd, inet_ntoa(client_info.sin_addr));
        clientdisplay(c);
        pthread_t id;
        if (pthread_create(&id, NULL, (void *)client_handler, (void *)c) != 0) {
            perror("Create pthread error!\n");
            exit(EXIT_FAILURE);
        }
    }
	
    return 0;
}

