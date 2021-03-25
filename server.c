#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>

char buffer[1024];

void setHttpHeader(char httpHeader[])
{
    // File object to return
    FILE *htmlData = fopen("test.html", "r");

    char line[100];
    char responseData[8000];
    while (fgets(line, 100, htmlData) != 0) {
        strcat(responseData, line);
    }
    strcat(httpHeader, responseData);
}

int main(int argc, char *argv[])
{
        char httpHeader[8000] = "HTTP/1.1 200 OK\r\n\n";
        int server_sock, client_sock;
        int listenfd;
        ssize_t len;
        pid_t childpid;
        int ret = 0;


        if(argc != 2)
        {
                printf("Usage: ./Program Port\n");
                exit(1);
        }
        int port_num = atoi(argv[1]);

        // Socket Creation Part
        if((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
                perror("socket error");
                exit(1);
        }

        // Addr Binding Part
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port_num);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if(bind(server_sock, (struct sockaddr *) &server_addr, 
                sizeof(server_addr)) < 0)
        {
                perror("bind error");
                close(server_sock);
                exit(1);
        }

        // Listen Part
        if((listenfd = listen(server_sock, 10)) < 0)
        {
                perror("listen error");
                close(server_sock);
                exit(1);
        }



        setHttpHeader(httpHeader);

        // Accept Part
        while(1)
        {
                if((client_sock = accept(server_sock, NULL, NULL)) < 0)
                {
                        perror("accept error");
                        close(server_sock);
                        exit(1);
                }
                send(client_sock, httpHeader, sizeof(httpHeader), 0);
                close(client_sock);
        }

        return 0;

}
