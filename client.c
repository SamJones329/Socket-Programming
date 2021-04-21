#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define CHAR_LIMIT 51
#define PORT 8080


int createSocket(int addrFam, char* addrStr) {
	
	int sock = 0, valread;
	struct sockaddr_in sa;

	if((sock = socket(addrFam, SOCK_STREAM, 0)) < 0) {
		printf("Socket creation error\n");
		return -1;
	}

	sa.sin_family = addrFam;
	sa.sin_port = htons(PORT);

	if(inet_pton(addrFam, addrStr, &sa.sin_addr) <= 0) {
		printf("Address invalid or not supported\n");
		return -1;
	}

	if(connect(sock, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
		printf("Connection failed\n");
		return -1;
	}

	return sock;

}

   
int main(int argc, char const *argv[]) {

	int sock = 0;
    int valread;
    struct sockaddr_in serv_addr;
    char *msg = (char*) calloc(CHAR_LIMIT, sizeof(char));
	char *addrStr = (char*) malloc(CHAR_LIMIT);
    char fromServer[1024] = {0};    

	//get server address
	printf("please input the IPv4 address of the server, or type '/exit' to end program: \n");
	fgets(addrStr, (50*sizeof(char)), stdin);
	
	if(strncmp(addrStr, "/exit", 5) == 0){
		free(addrStr);
		return 0; 
	}

	if((sock = createSocket(AF_INET, addrStr) < 0)) {
		printf("Something went wrong, please restart program to try again\n");
		exit(EXIT_SUCCESS);
	}
	
    //will loop to allow the client to send as many messages as desired
    while(1){
    
    	//ask for user input to determine message to send
	    printf("insert message to send to server, or type '/exit' to end program: \n");
	    fgets(msg, (50*sizeof(char)), stdin);
	    
	    if(strncmp(msg, "/exit", 5) == 0){
	    	free(msg);
	    	return 0; 
	    }
		
	    send(sock, msg, CHAR_LIMIT, 0 );
		printf("message sent: %s\n", msg);
		valread = read(sock, fromServer, 1024);
		printf("%s\n", fromServer);
    }

}
