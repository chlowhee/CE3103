#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include <strings.h>
#include <string.h>
#include <ctype.h>	//for isapha
#include <errno.h>	//for perror
#include <unistd.h>	//for read, write, close
#include <pthread.h>	//for multithreading

//int pthread_join(pthread_t thread1, void **retptr);	//return pointer to hole return value
//void pthread_exit(void *retptr);

void error(char*m){
	perror(m);
	exit(0);	//additional line to exit?
}

void *multiply(void *ptr){
	char *buffer2;
	buffer2 = (char*) ptr;
	int inputnum = atoi(buffer2);
	printf("Input number: %d\n", inputnum);

	int outputnum = inputnum * 5;
	printf("Output number: %d\n", outputnum);

	sprintf(buffer2, "%d", outputnum);	//convert from int to char array

	//pthread_exit(buffer);	//return output
}

int main (int argc, char *argv[]){
	int sockfd, newsockfd, port, clilen, n;
	int testBind = -1; //check if binding works
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int tempnum, tempnum5;

	pthread_t thread1;	//1 thread
	int T1ret;		//thread1 return

	void *status = 0;

	if(argc<2)
		error ("ERROR, no port provided\n");

	port = atoi(argv[1]);

	//Create Socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd <0)
		error ("ERROR opening socket\n");
	printf("Socket created\n");

	memset((char*) &serv_addr, 0, sizeof(serv_addr));

	//internet address family
	serv_addr.sin_family = AF_INET;
	//any incoming interface
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);	//host to network

	testBind = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof (serv_addr));
	if(testBind<0)
		error("ERROR binding to socket\n");
	printf("Socket binded\n");

	listen(sockfd, 2);

	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
	if (newsockfd <0)
		error("ERROR reading from socket\n");

	n = read(newsockfd, buffer, 255);
	if (n<0)
		error("ERROR reading from socket\n");

	printf("Message received from client\n");

	for(int i=0; i<strlen(buffer); i++){
		char numcheck = buffer[i];
		if(isalpha(numcheck)){
			printf("Mission abort. Invalid character.\n");
			exit(0);
		}
	}

	T1ret = pthread_create(&thread1, NULL, multiply, (void*) buffer);

	//pthread_join(thread1, (void**)&ret_val);	//get the new value
	pthread_join(thread1, &status);

	n = write(newsockfd, buffer, 255);
	if (n<0)
		error ("ERROR writing back to socket\n");

	printf("Connection ended\n");

	close(newsockfd);		//close the socket connections
	close(sockfd);
	sleep(1);


	return 0;
}

