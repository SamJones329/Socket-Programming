/**
*
*	This program will send a message to the server and receive its response, while logging all messages sent and recieved.
*
*	CSC Honors Programming Project
*
*	@since 4/20/2021
*	@authors George Buras, Samuel Jones, Adam Kardorff 
*
*/


#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define CHAR_LIMIT 51
#define PORT 8080





/**
 * This method updates the log file kept by the client. it will but the date and time a message was sent or recieved, 
 * as well as what the message was. 
 *
 *    routine: LogMsg
 *
 *    return type: void
 *
 *     parameters:
 *     char *msg:  the message that was sent or recieved, will be added to log
 *     char state: must be a char value of 's' or 'r', to indicate if the message was sent or recieved. (s = sent, r = recieved)
 *     
 *     @since 4/19/2021
 *     @author Adam Kardorff
 */
void LogMsg(char *msg, char state){
	
    FILE *fp = fopen("ClientLog.txt", "a");
	time_t t;
	time(&t);
	
	if(state == 's'){
		fprintf(fp, "%s message sent: %s\n", ctime(&t), msg);
	}
	else if(state == 'r'){
		fprintf(fp, "%s message recieved: %s\n\n", ctime(&t), msg);
	}
	else{
	 	printf("ERROR: incorrect format for char state\n");
	}

	fclose(fp);

}


    int main(int argc, char const *argv[]) {
    int sock = 0, valread;
    struct sockaddr_in sa;
    char *msg = (char*) calloc(CHAR_LIMIT, sizeof(char));
    char addrstr[CHAR_LIMIT] = "127.0.1.1";
    char addrlen;
    char fromServer[1024] = {0};    
	
	//get server address
	printf("please input the IPv4 address of the server, or type '/exit' to end program: ");
	fgets(addrstr, CHAR_LIMIT, stdin);
	    
	if(strncmp(addrstr, "/exit", 5) == 0){
		//free(addrstr);
		return 0; 
	}

	addrlen = strlen(addrstr);
	addrstr[addrlen-1] = '\0';
	// printf("Address Inputted: %s\n", addrstr);
	    
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Socket creation error\n");
		return -1;
	}

	sa.sin_family = AF_INET;
	sa.sin_port = htons(PORT);

	if(inet_pton(AF_INET, addrstr, &sa.sin_addr) <= 0) {
		printf("Address invalid or not supported\n");
		return -1;
	}

	if(connect(sock, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
		printf("Connection failed\n");
		return -1;
	}

	// Infinite loop to allow the client to send as many messages as desired
	while(1)
	{
		//ask for user input to determine message to send
		printf("insert message to send to server, or type '/exit' to end program (limit %d characters): \n", CHAR_LIMIT-1);
		fgets(msg, (50*sizeof(char)), stdin);
		
		
		send(sock, msg, (CHAR_LIMIT*sizeof(char)), 0);
		printf("message sent: %s\n", msg);
		LogMsg(msg, 's');
		
		// The /exit message will disconnect the client
		if(strncmp(msg, "/exit", 5) == 0)
		{
			free(msg);
			exit(0); 
		}			
		
		printf("about to read\n");
		valread = read(sock, fromServer, 1024);
		printf("im here\n");
		printf("%s\n", fromServer);
		LogMsg(fromServer, 'r');
	
	}
}
