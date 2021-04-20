// Server side C/C++ program to demonstrate Socket programming

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>

#define PORT 80
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[102] = {0};
    char *hello = "Hello from server";

    if (getenv("PROC") == NULL) { // If it's a parent process
    
        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        // Attaching socket to port 80
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons( PORT );

        // Forcefully attaching socket to the port 80
        if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
         
    	int pid = fork();
    	if (pid == 0) {
    	    // Child process
            char str_fd[5], env_fd[10] = "FD=";
            sprintf(str_fd, "%d", server_fd);
            strcat(env_fd, str_fd);
            putenv(env_fd);
            putenv("PROC=child");

            execl("./server", "./server", NULL);
            _exit(EXIT_FAILURE);
            
	} else if (pid > 0) {
            // Parent process
            wait(NULL);
            printf("Child returned\n");
		
        } else {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        
        close(server_fd);
        exit(0);
        
    } else if (strncmp(getenv("PROC"), "child", 5) == 0) {  // If it's a child process

        struct passwd* user_info = getpwnam("nobody"); // Get user id of nobody
        setuid(user_info->pw_uid); // Change user previlege to nobody from here
        
        int server_fd = atoi(getenv("FD"));
   
        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        valread = read( new_socket , buffer, 1024);
        printf("%s\n",buffer );
        send(new_socket , hello , strlen(hello) , 0 );
        printf("Hello message sent\n");

        _exit(0);

    } else {
        printf("Enviromental varialbe not properly set\n");
        exit(EXIT_FAILURE);
    }
}
