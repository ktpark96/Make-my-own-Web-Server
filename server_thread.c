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
#include <pthread.h>


#define CONNMAX 1000

int client[CONNMAX] = {-1, };
int now = 0;

char *ROOT;


void* respond(void *arg);

int main(int argc, char *argv[])
{
        int server_sock, client_sock;
        int listenfd;
        ssize_t len;
        pid_t childpid;
	
	pthread_mutex_t mutex;
	pthread_t tid;
        int n = 0, ret = 0;

	pthread_mutex_init(&mutex, NULL);
	

	ROOT = getenv("PWD");


        if(argc != 2)
        {
                printf("Usage: ./Program Port\n");
                exit(1);
        }
        int port_num = atoi(argv[1]);
	
//	for(int i = 0; i < CONNMAX; i++){client_sock[i] = -1;}

	
	
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

        // Accept Part
        while(1)
        {
                if((client_sock = accept(server_sock, NULL, NULL)) < 0)
                {
                        perror("accept error");
                        close(server_sock);
                        exit(1);
                }
				
	//	pthread_mutex_lock(&mutex);
	//	client[now++] = client_sock;
	//	pthread_mutex_unlock(&mutex);

		pthread_create(&tid, NULL, respond, (void*)&client_sock);
        	pthread_detach(tid);
	}

        return 0;

}


void * respond(void *arg)
{
	char msg[100000], *reqline[3], data_to_send[1024], path[99999];
	int rcvd, fd, bytes_read;
	
	int clnt_sock = *((int*)arg);
	
	memset((void *)msg, 0, 10000);

	rcvd = recv(clnt_sock, msg, 10000, 0);

	if (rcvd<0) { printf("Receive Error\n"); }
        else if (rcvd==0) { printf("Client Disconnected\n"); }
        else    // Receive Success
        {
                printf("%s\n", msg);
                reqline[0] = strtok (msg, " \t\n");
                
		if ( strncmp(reqline[0], "GET\0", 4)==0 )
                {
                        reqline[1] = strtok (NULL, " \t");
                        reqline[2] = strtok (NULL, " \t\n");
                        if ( strncmp( reqline[2], "HTTP/1.0", 8)!=0 && strncmp( reqline[2], "HTTP/1.1", 8)!=0 )
                        {
                                write(clnt_sock, "HTTP/1.0 400 Bad Request\n", 25);
                        }
                        else
                        {
                                if ( strncmp(reqline[1], "/\0", 2)==0 )
                                        reqline[1] = "/index.html";        //Because if no file is specified, index.html will be opened by default (like it happens in APACHE...

                                strcpy(path, ROOT);
                                strcpy(&path[strlen(ROOT)], reqline[1]);
                                printf("file: %s\n\n\n\n", path);

                                if ( (fd=open(path, O_RDONLY))!=-1 )    //FILE FOUND
                                {
                                        send(clnt_sock, "HTTP/1.0 200 OK\n\n", 17, 0);
                                        while ( (bytes_read=read(fd, data_to_send, 1024))>0 )
						write (clnt_sock, data_to_send, bytes_read);
                                }
                                else    write(clnt_sock, "HTTP/1.0 404 Not Found\n", 23); //FILE NOT FOUND
                        }
                }
        }
	shutdown(clnt_sock, SHUT_RDWR);
	close(clnt_sock);
	//clnt_sock = -1;
}
