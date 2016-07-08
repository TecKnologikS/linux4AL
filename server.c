/****************** SERVER CODE ****************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#define SIZE_COMMUNICATION 99998
#define BUFFER_SIZE 1024

char NAME[125] = "";
char PASSW[125] = "";
int STATE = 0;
int ERROR = 0;

char *trim (char *s) {
	int i = strlen(s) - 1;
	if ((i > 0) && (s[i] == '\n'))
		s[i] = '\0';
	return s;
}

void doProcess(int newSocket) {
	char buffer[BUFFER_SIZE];
	recv(newSocket, buffer, BUFFER_SIZE, 0);
	strcpy(buffer, trim(buffer));
	  /*---- Print the received message ----*/
	printf("Data received: %s, %i\n",buffer, STATE); 
	if (STATE == 2) {
		strcpy(PASSW, buffer);
		/*   Verifier si MDP CORRECT  */
		/*   si MDP CORRECT  */
		int correct = 0;
		
		FILE *f = fopen("/home/robin/mdp.txt", "r");
		char line[256];
		char ident[256];
		strcat(ident, NAME);
		strcat(ident, " - ");
		strcat(ident, PASSW);
		while(fgets(line, sizeof(line), f)) {
			if (strcmp(line, ident) == 0) {
				correct = 1;
				break;
			}
		}
		
		if (correct) {
			STATE = 3;
			strcpy(PASSW, buffer);
			strcpy(buffer,"WELC");
			send(newSocket,buffer,BUFFER_SIZE,0);
		} else {
			ERROR++;
			if (ERROR == 3) {
				strcpy(buffer,"BYE ");
				send(newSocket,buffer,BUFFER_SIZE,0);
				exit(1);
			} else {
				strcpy(buffer,"PASS?");
				send(newSocket,buffer,BUFFER_SIZE,0);
				STATE = 2;
			}
		}
		/*   si MDP INCORRECT  */
		/*   ERROR++;  */
		/*     */
	} else if (STATE == 1) {
		strcpy(NAME, buffer);
		strcpy(buffer,"PASS?");
		send(newSocket,buffer,BUFFER_SIZE,0);
		STATE = 2;
	} else if (strcmp(buffer, "BONJ") == 0 && STATE == 0) {
		STATE = 1;
		strcpy(buffer,"WHO?");
		send(newSocket,buffer,BUFFER_SIZE,0);
	} else if (STATE == 3) {
		if (strcmp(buffer, "EXIT") == 0) {
			strcpy(buffer,"BYE ");
			send(newSocket,buffer,BUFFER_SIZE,0);
			exit(1);
		} else if (strncmp(buffer, "rls", 3) == 0) {
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
			strcat(retour, "Files into actual directory \n");
			while(fgets(tmp, sizeof(tmp) -1, f) != NULL) {
				strcat(retour, tmp);
			}
			pclose(f);
			send(newSocket,retour,sizeof(retour),0);
				printf("Retour : %s \n", trim(retour));
		} else if (strcmp(buffer, "rcd") == 0) {
			strcpy(buffer, "Name of Directory ? \n");
			send(newSocket,buffer,BUFFER_SIZE,0);
			recv(newSocket, buffer, BUFFER_SIZE, 0);
			char tmp[] = "";
			strcpy(tmp, trim(buffer));
			if (chdir(tmp) == 0) {
				strcpy(buffer, "CDOk");
				printf("Actual directory : %s \n", tmp);
			} else {
				strcpy(buffer, "NOCD");
				printf("Error, can't change to this directory \n");
			}
			send(newSocket,buffer,sizeof(buffer),0);
		} else if (strcmp(buffer, "rpwd") == 0) {
			char retour[SIZE_COMMUNICATION];
			char command[] = "/bin/pwd ";
			char tmp[1024];
			FILE *f;
			f = popen(command, "r");
			if (f == NULL) {
				printf("Failed to run command \n");
				doProcess(newSocket);
			}
			strcat(retour, "Directory name: ");
			while(fgets(tmp, sizeof(tmp) -1, f) != NULL) {
				strcat(retour, tmp);
			}
			pclose(f);
			send(newSocket,retour,sizeof(retour),0);
		} else if (strcmp(buffer, "upld") == 0) {
			/*char retour[SIZE_COMMUNICATION];
			char tmp[1024];
			strcpy(buffer, "Name of file ? \n");
			send(newSocket,buffer,BUFFER_SIZE,0);
			recv(newSocket, buffer, BUFFER_SIZE, 0);
			printf("*** Download %s *** \n", trim(buffer));
			FILE *f;
			f = fopen(trim(buffer), "wb");
			if (f == NULL) {
				printf("*** Error Download  *** \n");
				strcat(retour, "FBDN");
			} else {
				strcat(buffer, "RDY");
				send(newSocket,retour,BUFFER_SIZE,0);
				char file[SIZE_COMMUNICATION];
				recv(newSocket, file, SIZE_COMMUNICATION, 0);
				FILE *f;
				f = fopen(trim(string), "wb");
				fprintf(f, "%s",  file);
				strcat(buffer, "File Upload OK");
				send(newSocket,retour,BUFFER_SIZE,0);
			}
			printf("*** End Download  *** \n");
			pclose(f);*/
		} else if (strcmp(buffer, "downl") == 0) {
			char retour[SIZE_COMMUNICATION];
			char tmp[1024];
			strcpy(buffer, "Name of file ? \n");
			send(newSocket,buffer,BUFFER_SIZE,0);
			recv(newSocket, buffer, BUFFER_SIZE, 0);
			printf("*** Download %s *** \n", trim(buffer));
			FILE *f;
			f = fopen(trim(buffer), "r");
			if (f == NULL) {
				printf("*** Error Download  *** \n");
				strcat(retour, "UNKNOW");
			} else {
				while(1) {
					if (fgets(tmp, 1024, f) == NULL) break;
					strcat(retour, tmp);
					printf("%s \n", tmp);
				}
			}
			printf("*** End Download  *** \n");
			pclose(f);
			send(newSocket,retour,SIZE_COMMUNICATION,0);
		} else {
			strcpy(buffer,"BAD_COMMAND");
			send(newSocket,buffer,BUFFER_SIZE,0);
		}
	} else {
		strcpy(buffer,"BAD_COMMAND");
		send(newSocket,buffer,BUFFER_SIZE,0);
	}
	doProcess(newSocket);
}

void sendHello(int newSocket) {
	char buffer[1024];
	
	strcpy(buffer,"WHO?\n");
	send(newSocket,buffer,6,0);
}

int main(){
  int welcomeSocket, newSocket;
  char buffer[1024];
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;
	pid_t pid;

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(7891);
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*---- Bind the address struct to the socket ----*/
  bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  /*---- Listen on the socket, with 5 max connection requests queued ----*/
  if(listen(welcomeSocket,5)==0)
    printf("Listening\n");
  else
    printf("Error\n");

  for(;;) {
	addr_size = sizeof serverStorage;
	while(1){
		newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
		if(newSocket < 0) {
			printf("Error on accept");
			exit(1);
		}
		pid = fork();
		if (pid < 0) {
			printf("Error on fork");
			exit(1);
		}
		
		if (pid == 0) {
			/* Child process */
			/* Do Action */
			doProcess(newSocket);
			//sendHello(newSocket);
			exit(0);
		} else {
			close(newSocket);
		}
	}
  }
  return 0;
}
