/* peer.c - main */

#include <sys/types.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>                                                                            
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
                  
#include <netdb.h>

#define	BUFSIZE 64

#define MY_IP     "10.1.1.49"

/*------------------------------------------------------------------------
 * main - Peer
 *------------------------------------------------------------------------
 */

struct cpdu {
	char type;
	char data[1024];
};

struct pdu {
	char type;
	char data[100];
};

int
main(int argc, char **argv)
{
	char	*host = "localhost";
	int	port = 3000;
	char	now[100];		/* 32-bit integer to hold time	*/ 
	struct  hostent	*phe;	/* pointer to host information entry	*/
	struct  sockaddr_in sin;	/* an Internet endpoint address		*/
	int	s, n, type;	/* socket descriptor and socket type	*/
	char    PEER_NAME[11];
	char    content_names[5][11];         /* Each peer has up to 5 content names  */
	int     content_index = 0;
	pid_t   child_pids[5] = {0};  // store child PIDs for each content
	int 	sockets[5];

	switch (argc) {
	case 1:
		break;
	case 2:
		host = argv[1];
	case 3:
		host = argv[1];
		port = atoi(argv[2]);
		break;
	default:
		fprintf(stderr, "usage: UDPtime [host [port]]\n");
		exit(1);
	}

	memset(&sin, 0, sizeof(sin));
	memset(content_names, 0, sizeof(content_names));
        sin.sin_family = AF_INET;                                                                
        sin.sin_port = htons(port);
                                                                                        
    /* Map host name to IP address, allowing for dotted decimal */
        if ( phe = gethostbyname(host) ){
                memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
        }
        else if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE )
		fprintf(stderr, "Can't get host entry \n");
                                                                                
    /* Allocate a socket */
        s = socket(AF_INET, SOCK_DGRAM, 0);
        if (s < 0)
		fprintf(stderr, "Can't create socket \n");
	
                                                                                
    /* Connect the socket */
        if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		fprintf(stderr, "Can't connect to %s %s \n", host, "Time");


	// For select	
	fd_set rfds, afds;
	FD_ZERO(&afds);
	FD_SET(0, &afds);    /* Listening on stdin */
	
	printf("\nEnter your name: ");
	scanf("%s", PEER_NAME);
	getchar();

	while(1){
		memcpy(&rfds, &afds, sizeof(rfds));

		printf("\n--------------------------------------------------------------\nMenu");
		printf("\n\t1: View list of available content");
		printf("\n\t2: Search Content");
		printf("\n\t3: Register Content");
		printf("\n\t4: De-Register Content");
		printf("\n\t0: Exit");
		printf("\nEnter command: ");
		fflush(stdout);


	        // Wait for either user input (stdin) or incoming connection
	        if (select(FD_SETSIZE, &rfds, NULL, NULL, NULL) < 0) {
			perror("select");
			continue;
	        }

		/* If user input is ready (stdin) */
    		if (FD_ISSET(0, &rfds)) {
			
			int input = 0;
			scanf("%d", &input);
	    		getchar();	

			switch (input) {
				case 1:{ //List
					printf("");
					struct pdu spdu;
					spdu.type = 'O';

					spdu.data[0] = '\0';
				    	write(s, &spdu, sizeof(spdu)); // Send entire struct

					printf("Receiving Content:");

					do{
						struct pdu rpdu;
						int bytes = read(s, &rpdu, sizeof(rpdu));
						if (rpdu.type == 'F' || rpdu.type == 'E'){
							if (rpdu.type == 'E'){
								printf("\n%s", rpdu.data);
							}
							break;				
						}else{
							  printf("\n%s", rpdu.data);
						}
					}while(1);
					break;
				}
				case 2:{ //Search
				    printf("\nEnter Content Name to Search: ");
				    struct pdu spdu;
				    spdu.type = 'S';

				    char content_name[10];
				    
				    fgets(content_name, sizeof(content_name), stdin);
				    content_name[strcspn(content_name, "\n")] = '\0';  // remove newline

				    // Fill PDU data: [0..9]=peer name, [10..19]=content name, [20..]=IP
				    //strncpy(spdu.data, PEER_NAME, 10);
				    strncpy(spdu.data, content_name, 10);
				    //strncpy(spdu.data + 20, strcat(MY_IP, reg_addr.sin_port), sizeof(spdu.data) - 20);

				    write(s, &spdu, sizeof(spdu));

				    struct pdu rpdu;
				    int bytes = read(s, &rpdu, sizeof(rpdu));
				    if (rpdu.type == 'E'){
					printf("\n%s", rpdu.data);
				    }else{
					char peer_name[11];
					char address[81];

					memcpy(peer_name, rpdu.data, 10);
					peer_name[10] = '\0';
					struct sockaddr_in client;
					socklen_t client_len = sizeof(client);
					memcpy(content_name, rpdu.data + 10, 10);
					content_name[10] = '\0';
					memcpy(address, rpdu.data + 20, 80);
					address[80] = '\0';

					printf("Content found: peer %s, content %s, address %s\n",
					       peer_name, content_name, address);
				    
					// MAKE TCP CONNECTION WITH CONTENT SERVER

					char host_addr[64];
					int peer_port;

					char *colon = strchr(address, ':');
					if (colon == NULL) {
					    fprintf(stderr, "Invalid address format: %s\n", address);
					    // handle error
					} else {
					    *colon = '\0';  // terminate host part
					    strcpy(host_addr, address);
					    peer_port = atoi(colon + 1);
					}

					printf("Host: %s\n", host_addr);
					printf("Port: %d\n", peer_port);

					int sd;
					struct sockaddr_in peer_addr;
					struct hostent *hp;

					sd = socket(AF_INET, SOCK_STREAM, 0);
					if (sd < 0) {
					    perror("socket");
					    break;
					}

					memset(&peer_addr, 0, sizeof(peer_addr));
					peer_addr.sin_family = AF_INET;
					peer_addr.sin_port = htons(peer_port);

					if (hp = gethostbyname(host_addr)){ 
					  bcopy(hp->h_addr, (char *)&peer_addr.sin_addr, hp->h_length);
					  printf("Resolved %s via DNS: %s\n", host_addr, inet_ntoa(*(struct in_addr *)hp->h_addr));
					}
					else if (inet_aton(host_addr, (struct in_addr *) &peer_addr.sin_addr) ==0){
					  fprintf(stderr, "Can't get server's address\n");
					  exit(1);
					}

				        printf("Connecting to %s:%d...\n", inet_ntoa(peer_addr.sin_addr), peer_port);

					if (connect(sd, (struct sockaddr *)&peer_addr, sizeof(peer_addr)) < 0) {
					    fprintf(stderr, "Can't connect \n");
					    close(sd);
					    break;
					}

					printf("Connected to peer %s at %s:%d\n", peer_name, host_addr, peer_port);

					// Open a file in writing mode
					  char	rbuf[1024];
					  FILE *fptr = fopen(content_name, "w");

					  while ((n = read(sd, rbuf, 1024)) > 0) {
						//rbuf[n] = '\0';  // null-terminate just in case
						fprintf(fptr, "%s", rbuf);
					  }

					  // Close the file
					  fclose(fptr); 
					  close(sd);
				    }

				    break;

				
				}
				case 3:{
				    printf("\nEnter Content Name to Register: ");
				    struct pdu spdu;
				    spdu.type = 'R';

				    char content_name[10];
				    
				    fgets(content_name, sizeof(content_name), stdin);
				    content_name[strcspn(content_name, "\n")] = '\0';  // remove newline

		
				    struct sockaddr_in reg_addr;
				    int sd = socket(AF_INET, SOCK_STREAM, 0);
				    sockets[content_index] = sd;
				    strncpy(content_names[content_index], content_name, sizeof(content_names[content_index]));
				    reg_addr.sin_family = AF_INET;
				    reg_addr.sin_port = htons(0);
				    reg_addr.sin_addr.s_addr = htonl(INADDR_ANY);
				    
				    if (bind(sd, (struct sockaddr *)&reg_addr, sizeof(reg_addr)) < 0) {
					    perror("bind");
					    close(sd);
					    break;
				    }

				    if (listen(sd, 5) < 0) {
					    perror("listen");
					    close(sd);
					    break;
				    }

				    FD_SET(sd, &afds);
				

				    socklen_t alen = sizeof (struct sockaddr_in);
				    getsockname(sd, (struct sockaddr *) &reg_addr, &alen);
	
				    //Fork and Listen for Download Requests

				    // Fill PDU data: [0..9]=peer name, [10..19]=content name, [20..]=IP
				    char my_address[81];
				    snprintf(my_address, sizeof(my_address), "%s:%d", MY_IP, ntohs(reg_addr.sin_port));
				    strncpy(spdu.data, PEER_NAME, 10);
				    strncpy(spdu.data + 10, content_name, 10);
				    strncpy(spdu.data + 20, my_address, 80);
				    write(s, &spdu, sizeof(spdu));

				    struct pdu rpdu;
				    int bytes = read(s, &rpdu, sizeof(rpdu));
				    if (rpdu.type == 'E'){
					printf("\n%s", rpdu.data);
					close(sd);
				    }else{
					printf("Registered content '%s'\n", content_name);
					content_index++;
				    }

				    pid_t pid = fork();
				    if (pid == 0) { // child process
				    while(1) {
					struct sockaddr_in client;
					socklen_t client_len = sizeof(client);
				        int new_sd = accept(sd, (struct sockaddr *)&client, &client_len);
					if (new_sd >= 0) {
					        char fixedPath[] = "/home/condor/Desktop/";

						char fullFilePath[512];
						snprintf(fullFilePath, sizeof(fullFilePath), "%s%s", fixedPath, content_name);
						printf("Requested file: %s\n", fullFilePath);
						fflush(stdout);

						FILE* file_pointer = fopen(fullFilePath, "r");
						printf("Reading contents of: %s\n", fullFilePath);
						fflush(stdout);
				 
						char buffer[1024];
						while (fgets(buffer, sizeof(buffer), file_pointer) != NULL) {
							write(new_sd, buffer, strlen(buffer));
							//printf("\n Sent 100 bytes");
							//fflush(stdout);
						}

						fclose(file_pointer);

						close(new_sd);
					}
				    }
				    exit(0); // never reached
				}else { child_pids[content_index-1] = pid;}


				    break;
				}
				case 4:{
				    printf("\nEnter Content Name to De-Register: ");
				    struct pdu spdu;
				    spdu.type = 'T';

				    char content_name[10];
				    
				    fgets(content_name, sizeof(content_name), stdin);
				    content_name[strcspn(content_name, "\n")] = '\0';  // remove newline

				    // Fill PDU data: [0..9]=peer name, [10..19]=content name, [20..]=IP
				    strncpy(spdu.data, PEER_NAME, 10);
				    strncpy(spdu.data + 10, content_name, 10);
				    //strncpy(spdu.data + 20, MY_IP, sizeof(spdu.data) - 20);

				    write(s, &spdu, sizeof(spdu));

				    struct pdu rpdu;
				int bytes = read(s, &rpdu, sizeof(rpdu));
				if (rpdu.type == 'E'){
					printf("\n%s", rpdu.data);
				}else{
					int i = 0;
					for (i; i<5; i++){
						if (strcmp(content_names[i], content_name) == 0){
							break;
						}
					}
					close(sockets[i]);
					FD_CLR(sockets[i], &afds);
					// Kill the child process serving this content
					if (child_pids[i] > 0) {
					    kill(child_pids[i], SIGTERM); 
					    waitpid(child_pids[i], NULL, 0); // reap zombie
					}
					for (i; i<(content_index-1); i++){
						strcpy(content_names[i], content_names[i+1]);
						sockets[i] = sockets[i+1];
						child_pids[i] = child_pids[i+1];
					}
					content_index--;
					printf("De-Registered content '%s'\n", content_name);
				}

				    break;
				}
				case 0:{
					printf("");
					struct pdu spdu;
					spdu.type = 'Q';
					strncpy(spdu.data, PEER_NAME, 10);
					spdu.data[10] = '\0';
				    	write(s, &spdu, sizeof(spdu));
					
					int i=0;
					for (i; i<(content_index); i++){
						if (child_pids[i] > 0) {
								kill(child_pids[i], SIGTERM); 
								waitpid(child_pids[i], NULL, 0); // reap zombie
							}
					}
					printf("Quitting program.\n");
					exit(1);
				}
				default:
					printf("Unknown command: %d\n", input);
			}
		}		

	}

	exit(0);
}
