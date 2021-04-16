// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

#define PORT 8080

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
    char *hello = "Hello from server";

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

    while(1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                        (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        if (Fork() == 0)
        {
            while(1)
            {   
                valread = read( new_socket , buffer, 1024);

                if(strncmp(buffer, "/exit", 5) == 0)
                {
                    exit(0); 
                }
                else
                {
                    printf("%s\n",buffer );
                    send(new_socket , hello , strlen(hello) , 0 );
                    printf("Hello message sent\n");
                }
            }
        }
    }
}
