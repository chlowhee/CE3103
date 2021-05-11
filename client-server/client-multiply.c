#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
//#include <strings.h>	//for bcopy?
#include <stdlib.h>
#include <ctype.h>	//for isalpha
#include <unistd.h>	//for read, write, close
#include <errno.h>	//for perror

void error (char*m){
	perror(m);
	exit(0);
}

int main(int argc, char *argv[]){
	int sockfd, port, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256];
	//int tempnum;

	if (argc<3)
		error("usage client [hostname] [port]\n");

	port = atoi(argv[2]);
	sockfd = socket (AF_INET, SOCK_STREAM, 0);
	if (sockfd <0)
		error("ERROR opening socket\n");

	server = gethostbyname(argv[1]);
	if (server == NULL)
		error("ERROR, no such host\n");

	//start with a clean address structure
	memset((char*) &serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;		//internet socket
	bcopy((char*) server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);

	//if(connect(sockfd, &serv_addr, sizeof(serv_addr))<0)
	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
		error("ERROR connecting");

	printf("Please enter a number: ");
	fgets(buffer, 255, stdin);	//user input message

	for(int i=0; i<strlen(buffer); i++){
		char numcheck = buffer[i];
		if(isalpha(numcheck)){
			printf("Error! Numbers only!\n");
			n = write(sockfd, "Invalid Number", 14);
			exit(1);
		}
	}

	n = write(sockfd, buffer, strlen(buffer));
	if(n<0)
		error("ERROR writing from socket");

	n = read(sockfd, buffer, 255);		//read the number received from server
	if (n<0)
		error("ERROR reading from socket");

	printf("Answer: %s\n", buffer);

	close(sockfd);		//close the socket connections
	shutdown(sockfd, 0);
	shutdown(sockfd, 1);
	shutdown(sockfd, 2);

	return 0;
}
