/****************** CLIENT CODE ****************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE_COMMUNICATION 99998
#define BUFFER_SIZE 1024
int CONNECTED = 0;
char buffer[1024];

void Receive(int newSocket);
char *trim (char *s);
void doProcess(int newSocket);

char *trim (char *s) {
	int i = strlen(s) - 1;
	if ((i > 0) && (s[i] == '\n'))
		s[i] = '\0';
	return s;
}

void doProcess(int newSocket) {
	char string[256];
	fgets(string, 254, stdin);
	char tmp[] = "";
	strcpy(tmp, trim(string));
	if (strcmp(tmp, "cd") == 0) {
		printf("Name of Directory ? \n");
		fgets(string, 254, stdin);
		char tmp[] = "";
		strcpy(tmp, trim(string));
		if (chdir(tmp) == 0) {
			printf("Actual directory : %s \n", tmp);
		} else {
			printf("Error, can't change to this directory \n");
		}
		doProcess(newSocket);
	} else if (strcmp(tmp, "ls") == 0) {
		char retour[SIZE_COMMUNICATION];
		char command[] = "/bin/ls ";
		char tmp[1024];
		//strcat(command, PATH);
		FILE *f;
		f = popen(command, "r");
		if (f == NULL) {
			printf("Failed to run command \n");
			doProcess(newSocket);
		}
		while(fgets(tmp, sizeof(tmp) -1, f) != NULL) {
			strcat(retour, tmp);
		}
		pclose(f);
		printf("******************************* \n");
		printf(" File into Directory \n");
		printf("******************************* \n");
		printf("%s ", retour);
		printf("******************************* \n");
		doProcess(newSocket);
	} else if (strcmp(tmp, "pwd") == 0) {
		char retour[SIZE_COMMUNICATION];
		char command[] = "/bin/pwd ";
		char tmp[1024];
		FILE *f;
		f = popen(command, "r");
		if (f == NULL) {
			printf("Failed to run command \n");
			doProcess(newSocket);
		}
		while(fgets(tmp, sizeof(tmp) -1, f) != NULL) {
			strcat(retour, tmp);
		}
		pclose(f);
		printf("******************************* \n");
		printf(" Name of actual Directory : %s ", retour);
		printf("******************************* \n");
		doProcess(newSocket);
	} else if (strcmp(tmp, "rm") == 0) {
		printf("Name of file to delete ? \n");
		fgets(string, 254, stdin);
		char tmp[] = "";
		strcpy(tmp, trim(string));
		if (unlink(tmp) == 0) {
			printf("Delete OK \n");
		} else {
			printf("Error, can't delete it \n");
		}
		doProcess(newSocket);
	} else if (strcmp(tmp, "downl") == 0) {
		strcpy(buffer, trim(tmp));
		send(newSocket,buffer,BUFFER_SIZE,0);
		recv(newSocket, buffer, SIZE_COMMUNICATION, 0);
		printf("%s \n",trim(buffer)); 
		fgets(string, 254, stdin);
		send(newSocket,trim(string),1024,0);
		char file[SIZE_COMMUNICATION];
		recv(newSocket, file, SIZE_COMMUNICATION, 0);
		FILE *f;
		f = fopen(trim(string), "wb");
		if (f != NULL) {
			fprintf(f, "%s",  file);
		} else {
			printf("Error download");
		}
		fclose(f);
		printf("File downloaded in actual file \n");
		doProcess(newSocket);
	} else if (strcmp(tmp, "upld") == 0) {
		strcpy(buffer, trim(tmp));
		send(newSocket,buffer,BUFFER_SIZE,0);
		recv(newSocket, buffer, BUFFER_SIZE, 0);
		fgets(string, 254, stdin);
		send(newSocket,trim(string),BUFFER_SIZE,0);
		recv(newSocket, buffer, BUFFER_SIZE, 0);
		if (strcmp(trim(buffer), "RDY") == 0) {
			char retour[SIZE_COMMUNICATION];
			char tmp[1024];
			/*while(1) {
				if (fgets(tmp, 1024, f) == NULL) break;
				strcat(retour, tmp);
			}
			pclose(f);*/
			send(newSocket,retour,SIZE_COMMUNICATION,0);
			Receive(newSocket);
		} else {
			printf("Error upload");
			doProcess(newSocket);
		}
	} else {
		strcpy(buffer,string);
		send(newSocket,buffer,BUFFER_SIZE,0);
		Receive(newSocket);
	}
}

void Receive(int newSocket) {
	/*---- Read the message from the server into the buffer ----*/
	recv(newSocket, buffer, SIZE_COMMUNICATION, 0);
	/*---- Print the received message ----*/
	printf("%s \n",trim(buffer)); 
	if (strncmp(buffer, "BYE", 3) == 0) {
		exit(1);
	}
	doProcess(newSocket);
}


int main(){
  int clientSocket;
  char buffer[BUFFER_SIZE];
  struct sockaddr_in serverAddr;
  socklen_t addr_size;

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(7891);
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*---- Connect the socket to the server using the address struct ----*/
  addr_size = sizeof serverAddr;
  if (connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size) < 0) {
	printf("Error on accept \n");
	exit(1);
  }
  strcpy(buffer,"BONJ");
  send(clientSocket,buffer,BUFFER_SIZE,0);
  Receive(clientSocket);

  return 0;
}