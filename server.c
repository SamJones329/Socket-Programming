/**
*
*	This server will receive messages from a client and send a response, while logging all messages sent and received
*
*	CSC Honors Programming Project
*
*	@since 4/20/2021
*	@authors George Buras, Samuel Jones, Adam Kardorff 
*
*/



#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>

#define PORT 8080


   /**
   * This method updates the log file kept by the server. it will but the date and time a message was sent or recieved, 
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
	
    FILE *fp = fopen("ServerLog.txt", "a");
	time_t t;
	time(&t);
	
	if(state == 's'){
		fprintf(fp, "%s message sent: %s\n\n", ctime(&t), msg);
	}
	else if(state == 'r'){
		fprintf(fp, "%s message recieved: %s\n", ctime(&t), msg);
	}
	else{
	 	printf("ERROR: incorrect format for char state\n");
	}

	fclose(fp);

}

/**
 * This method is an error-reporting function, used to report a unix-style error.
 * In this program it is used in the Fork method to report errors caused when calling fork.
 * 
 * routine: UnixError
 * 
 * return type: void
 * 
 * parameters:
 *      msg     [char*]     the message to be displayed when an error is found
 * 
 * @author George Adler Buras
 * @since 4/7/2021
 * 
 */
void UnixError(char* msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}

/**
 * This method is a Stevens-style error-handling wrapper used to simplify
 * error reporting when calling fork
 * 
 * routine: Fork
 * 
 * return type: pid_t
 * 
 * @author George Adler Buras
 * @since 4/7/2021
 * 
 */
pid_t Fork(void)
{
    pid_t pid;

    if ((pid = fork()) < 0)
        UnixError("Fork error");
    return pid;
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Confimation message
    char *confirm = "Message received";

    // String used to check input for the /quit command
    char *quit;
    quit = (char*) calloc(51, sizeof(char));

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, 
                                sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Forks to look for quit command while handling clients
    if (Fork() == 0)
    {
        // Infinite loop to allow multiple clients to connect to the same server
        while(1)
        {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                            (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            // The child process handles the client connection while the parent looks for new clients
            if (Fork() == 0)
            {
                // Infinite loop to allow as many messages to be sent as desired
                while(1)
                {   
                    valread = read( new_socket , buffer, 1024);
                    LogMsg(buffer, 'r');

                    // The /exit message is sent when the client disconnects from the server
                    if (strncmp(buffer, "/exit", 5) == 0)
                    {
                        printf("Client has disconnected\n");
                        exit(0);
                    }

                    printf("%s\n",buffer );
                    send(new_socket , confirm , strlen(confirm) , 0 );
                    LogMsg(confirm, 's');
                    printf("Confirmation message sent\n");
                }
            }
        }
    }

    do {
        printf("Server active\nType '/quit' to terminate\n");
        fgets(quit, (50*sizeof(char)), stdin);
        //printf("%s\n", quit);
    } while(strncmp(quit, "/quit", 5) != 0);

    free(quit);
    exit(0);
}
