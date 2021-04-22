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
#define EXIT "Client exiting"




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
	else if(state == 'e') {
        fprintf(fp, "%s at %s\n", msg, ctime(&t));
    }
	else{
	 	printf("ERROR: incorrect format for char state\n");
	}

	fclose(fp);

}


int main(int argc, char const *argv[]) {
    int sock = 0, valread;
    struct sockaddr_in sa;
    char *msg = (char*) calloc(2*CHAR_LIMIT-1, sizeof(char));
	char name[CHAR_LIMIT] = {0};
	char namelen;
	char input[CHAR_LIMIT] = {0};
	char inputlen;
    char addrstr[CHAR_LIMIT] = "127.0.1.1";
    char addrlen;
    char fromServer[CHAR_LIMIT] = {0};
	char exitlen = strlen(EXIT);
	
	printf("Input '/exit' at any time to exit the program.\n");

	//get client name
	printf("Input name of this client: ");
	fgets(name, CHAR_LIMIT, stdin);
	namelen = strlen(name);
	name[namelen-1] = '\0';

	//get server address
	printf("Input the IPv4 address of the server: ");
	fgets(addrstr, CHAR_LIMIT, stdin);
	    
	if(strncmp(addrstr, "/exit", 5) == 0){
		return EXIT_SUCCESS; 
	}

	addrlen = strlen(addrstr);
	addrstr[addrlen-1] = '\0';
	    
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Socket creation error\n");
		return EXIT_FAILURE;
	}

	sa.sin_family = AF_INET;
	sa.sin_port = htons(PORT);

	if(inet_pton(AF_INET, addrstr, &sa.sin_addr) <= 0) {
		printf("Address invalid or not supported\n");
		return EXIT_FAILURE;
	}

	if(connect(sock, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
		printf("Connection failed\n");
		return EXIT_FAILURE;
	}

	// Infinite loop to allow the client to send as many messages as desired
	while(1)
	{
		//ask for user input to determine message to send
		printf("\nInput message to send to server (%d char max): ", CHAR_LIMIT-1);
		fgets(input, CHAR_LIMIT-1, stdin);
		inputlen = strlen(input);
		input[inputlen-1] = '\0';
		
		sprintf(msg, "%s: %s", name, input);
		
		send(sock, msg, (CHAR_LIMIT*sizeof(char)), 0);
		printf("Message sent: \"%s\"\n", msg);
		LogMsg(msg, 's');
		
		// The /exit message will disconnect the client
		if(strncmp(input, "/exit", 5) == 0)
		{
			free(msg);
			LogMsg(EXIT, 'e');
			exit(EXIT_SUCCESS); 
		}			
		
		valread = read(sock, fromServer, CHAR_LIMIT);
		printf("Response: %s\n", fromServer);
		LogMsg(fromServer, 'r');
	
	}
}
