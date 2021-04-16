#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define CHAR_LIMIT 51
#define PORT 8080
   
int main(int argc, char const *argv[])
{
    int sock = 0;
    int valread;
    struct sockaddr_in serv_addr;
    char *msg;
    msg = (char*) calloc(CHAR_LIMIT, sizeof(char));
    char fromServer[1024] = {0};    
	
    //will loop to allow the client to send as many messages as desired
    while(1)
	{
    
		//ask for user input to determine message to send
	    printf("insert message to send to server, or type '/exit' to end program (limit %d characters): \n", CHAR_LIMIT-1);
	    fgets(msg, (50*sizeof(char)), stdin);
	    
	    if(strncmp(msg, "/exit", 5) == 0){
	    	free(msg);
	    	return 0; 
	    }
	    

	    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	    {
			printf("\n Socket creation error \n");
			return -1;
	    }
	   
	    serv_addr.sin_family = AF_INET;
	    serv_addr.sin_port = htons(PORT);
	       
	    // Convert IPv4 and IPv6 addresses from text to binary form
	    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
	    {
			printf("\nInvalid address/ Address not supported \n");
			return -1;
	    }
	   
	    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	    {
			printf("\nConnection Failed \n");
	    }
	    else
		{
	    	send(sock , msg , (CHAR_LIMIT*sizeof(char)) , 0 );
			printf("message sent: %s\n", msg);
			valread = read( sock , fromServer, 1024);
			printf("%s\n",fromServer );
	    }
    }


}
