#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#define PORT 8080

void setHttpHeader(char httpHeader[])
{
	// File object to return
    	FILE *htmlData = fopen("index.html", "r");

    	char line[100];
	char responseData[8000];
	while (fgets(line, 100, htmlData) != 0) 
	{
        	strcat(responseData, line);
    	}
	// char httpHeader[8000] = "HTTP/1.1 200 OK\r\n\n";
	strcat(httpHeader, responseData);
}

int main(int argc, char* argv[])
{
	char httpHeader[8000] = "HTTP/1.1 200 OK\r\n\n";
	
	int server_sock, client_sock;

	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	socklen_t client_addr_size;
	
	// socket()
	if((server_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Socket Fail");
		exit(1);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = PF_INET;			// IPV4
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// IP_ADDR
	server_addr.sin_port = htons(PORT);			// PORT
	
	// bind()
	if(bind(server_sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1)
	{
		perror("Bind Fail");
		exit(1);
	}

	if(listen(server_sock, 10) == -1)
	{
		perror("Listen Fail");
		exit(1);
	}
	
	setHttpHeader(httpHeader);
	
	memset(&client_addr, 0, sizeof(client_addr));

	while(1)
	{
		//client_addr_size = sizeof(client_addr);
		client_sock = accept(server_sock,NULL, NULL);
		send(client_sock, httpHeader, sizeof(httpHeader), 0);
		close(client_sock);
	}
	
	//client_addr_size = sizeof(client_addr);
	//client_sock = accept(server_sock, (struct sockaddr*) &client_addr, &client_addr_size);
/*
	if(client_sock == -1)
	{
		perror("Accept Failed");
		exit(0);
	}
*/
	

//	char msg[] = "Hello this is server!\n";
//	write(client_sock, msg, sizeof(msg));

	//close(client_sock);
	//close(server_sock);

	return 0;



}
