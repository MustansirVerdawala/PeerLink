/* index.c - main */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <time.h>


/*------------------------------------------------------------------------
 * main - Index server
 *------------------------------------------------------------------------
 */

struct pdu {
	char type;              /* Type of message (R, S, T, O, E, etc.) */
	char data[100];        /* Message payload                       */
};

int
main(int argc, char *argv[])
{
	struct  sockaddr_in fsin;	/* Address of a connected client         */
	char	buf[100];		/* General-purpose buffer (unused)       */
	char    *pts;
	int	sock;			/* Unused socket variable                */
	time_t	now;			/* Current time variable (unused)        */
	int	alen;			/* Client address length                 */
	struct  sockaddr_in sin; 	/* Server socket address structure       */
        int     s, type;        	/* Socket descriptor and type (UDP)      */
	int 	port=3000;              /* Default port number                   */

	/* Data structures to store peer info */
	char    peer_names[7][11];               /* Array of up to 7 peer names          */
	int     peer_names_index = 0;            /* Number of registered peers           */
	char    content_names[7][5][11];         /* Each peer has up to 5 content names  */
	int     content_names_index[] = {0, 0, 0, 0, 0, 0, 0}; /* Per-peer content counters */
	char    addresses[7][5][81];                /* Stores address string for each peer  */

	int     content_name_request_list[7][5];
	int     current_minimum;
	

	memset(peer_names, 0, sizeof(peer_names));
	memset(content_names, 0, sizeof(content_names));
	memset(addresses, 0, sizeof(addresses));
	memset(content_name_request_list, 0, sizeof(content_name_request_list));
	

	// How do I track last_used_status???? (to be implemented)

	/* Handle optional port argument */
	switch(argc){
		case 1:
			break;
		case 2:
			port = atoi(argv[1]); /* Use user-specified port */
			break;
		default:
			fprintf(stderr, "Usage: %s [port]\n", argv[0]);
			exit(1);
	}

        /* Initialize socket structure */
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;              /* Internet family */
        sin.sin_addr.s_addr = INADDR_ANY;      /* Accept any incoming address */
        sin.sin_port = htons(port);            /* Convert port to network byte order */
                                                                                                 
    	/* Create UDP socket */
        s = socket(AF_INET, SOCK_DGRAM, 0);
        if (s < 0)
		fprintf(stderr, "can't create socket\n");
                                                                                
    	/* Bind the socket to the port */
        if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		fprintf(stderr, "can't bind to %d port\n",port);

        // listen(s, 5);	/* (Note: not needed for UDP) */
	alen = sizeof(fsin);

	/* Main server loop - waits for messages */
	while (1) {
		fflush(stdout);
		struct pdu rpdu; /* Receive PDU from client */
		if (recvfrom(s, &rpdu, sizeof(rpdu), 0,
				(struct sockaddr *)&fsin, &alen) < 0)
			fprintf(stderr, "recvfrom error\n");

	 	switch(rpdu.type){ /* Check PDU type */
			case 'Q': {
			    char peer_name[11];
			    strncpy(peer_name, rpdu.data, 10);
			    peer_name[10] = '\0';

			    printf("\n\n\nDeregister ALL Request: Peer: %s", peer_name);

			    int found = 0;
			    int peer_index = 0;

			    // Find the peer
			    for (peer_index = 0; peer_index < peer_names_index; peer_index++) {
				if (strcmp(peer_names[peer_index], peer_name) == 0) {
				    found = 1;
				    break;
				}
			    }

			    struct pdu spdu;

			    if (!found) {
				// Peer not found
				spdu.type = 'E';
				snprintf(spdu.data, sizeof(spdu.data), "Error: Peer %s not found", peer_name);
				printf("\n\tPeer not found!");
			    } else {
				// Deregister all content
				content_names_index[peer_index] = 0;  // Reset content count
				memset(content_names[peer_index], 0, sizeof(content_names[peer_index]));
				memset(addresses[peer_index], 0, sizeof(addresses[peer_index]));

				spdu.type = 'A';
				snprintf(spdu.data, sizeof(spdu.data), "All content deregistered for peer %s", peer_name);
				printf("\n\tAll content deregistered for peer %s!", peer_name);
			    }

			    // Send response
			    //(void) sendto(s, &spdu, sizeof(spdu), 0, (struct sockaddr *)&fsin, sizeof(fsin));
			    break;

			}
			case 'R': {/* Registration request */

				printf("");

				int temp_peer_index = peer_names_index; /* Temporary index for peer */
				int flag  = 0; /* Flag: peer exists? */
				int flag2 = 0; /* Flag: content duplicate? */

				char peer_name[11]; /* Extract peer name */
				strncpy(peer_name, rpdu.data, 10);
				peer_name[10] = '\0';

				char content_name[11]; /* Extract content name */
				strncpy(content_name, rpdu.data + 10, 10);
				content_name[10] = '\0';

				char address[81]; /* Extract address string */
				strncpy(address, rpdu.data + 20, 80);
				address[80] = '\0';

				struct pdu spdu;

				printf("\n\n\nRegistration Request: \n\tPeer: %s \n\tContent:%s \n\tAddress:%s", peer_name, content_name, address);
				
				/* Check if peer already exists */
				int i = 0;
				for (i; i<peer_names_index; i++){
					if (strcmp(peer_names[i], peer_name) == 0){
						temp_peer_index = i;
						flag = 1; /* Peer found */
						printf("\n\t\tPeer already exists!");
						break;
					}
				}

				if (!flag){ /* New peer registration */
					if (peer_names_index >=7){
						spdu.type = 'E';
						strncpy(spdu.data, "Max Peers Reached!", sizeof(spdu.data));
						printf("\n\t\tMax Peers Reached!");
					}else{
						strncpy(peer_names[peer_names_index], peer_name, sizeof(peer_names[0]));
						peer_names[peer_names_index][10] = '\0';

						strncpy(content_names[peer_names_index][content_names_index[peer_names_index]], content_name, sizeof(content_names[0][0]));
						content_names[peer_names_index][content_names_index[peer_names_index]][10] = '\0';

						strncpy(addresses[peer_names_index][content_names_index[peer_names_index]], address, sizeof(addresses[0]));
						addresses[peer_names_index][content_names_index[peer_names_index]][80] = '\0';

						content_names_index[peer_names_index]++;					
						peer_names_index++;

						char msg[100];
						snprintf(msg, sizeof(msg), "Acknowledged Peer %s with content %s:\n", peer_name, content_name);
						spdu.type = 'A';
						strncpy(spdu.data, msg, sizeof(spdu.data));
						printf("\n\t\tREGISTRATION SUCCESSFUL!!");
					}
						
				}else{ /* Existing peer - check for duplicate content */
					int i = 0;
					for (i; i<content_names_index[temp_peer_index]; i++){
						if (strcmp(content_names[temp_peer_index][i], content_name) == 0){
							flag2 = 1; /* Duplicate found */
							printf("\n\tDuplicate found!");
							break;
						}
					}
					if (flag2){ /* Send error if duplicate content */
						char errorMsg[100];
						snprintf(errorMsg, sizeof(errorMsg),
							"Error: Peer with same content alr exists:\n");
						spdu.type = 'E';
						strncpy(spdu.data, errorMsg, sizeof(spdu.data));
						printf("\n\tPeer with same content alr exists!");
						
					}else{ /* Add new content to existing peer */
						if (content_names_index[temp_peer_index]>=5){
							spdu.type = 'E';
							strncpy(spdu.data, "Max Content Reached!", sizeof(spdu.data));
						}else{
							char msg[100];
							snprintf(msg, sizeof(msg),
								"Acknowledged Peer %s with content %s:\n", peer_name, content_name);
							spdu.type = 'A';
							strncpy(spdu.data, msg, sizeof(spdu.data));
							
							/* Add content to peer’s content list */
							strncpy(peer_names[temp_peer_index], peer_name, sizeof(peer_names[0]));
							peer_names[temp_peer_index][10] = '\0';

							strncpy(content_names[temp_peer_index][content_names_index[temp_peer_index]], content_name, sizeof(content_names[0][0]));
							content_names[temp_peer_index][content_names_index[temp_peer_index]][10] = '\0';

							strncpy(addresses[temp_peer_index][content_names_index[temp_peer_index]], address, sizeof(addresses[0]));
							addresses[temp_peer_index][content_names_index[temp_peer_index]][80] = '\0';

							content_names_index[temp_peer_index]++;
							printf("\n\t\tREGISTRATION SUCCESSFUL!!");
						}
					}
				}
				(void) sendto(s, &spdu, sizeof(spdu), 0, (struct sockaddr *)&fsin, sizeof(fsin));
				
				// set last_used_status to 0 here (placeholder for activity tracking)

				break;
			}
			case 'S': /* Search request */{
				printf("");

				int temp_peer_index = 0; /* Temporary index for peer */
				int temp_content_index = 0;

				int flag  = 0; /* Flag: content exists? */
				
				//char peer_name[11]; /* Extract peer name */
				//strncpy(peer_name, rpdu.data, 10);
				//peer_name[10] = '\0';

				char content_name[11]; /* Extract content name */
				strncpy(content_name, rpdu.data, 10);
				content_name[10] = '\0';

				printf("\n\n\nSearch Request: \n\tContent:%s", content_name);
				
				int counter1;
				int counter2;
				struct pdu spdu;
				
				for (counter1=0; counter1 < peer_names_index; counter1++){
					for (counter2=0; counter2 < content_names_index[counter1]; counter2++){
						if (strcmp(content_name, content_names[counter1][counter2])==0){
							if (!flag){
								current_minimum = content_name_request_list[counter1][counter2];
							}

							flag = 1;
							if (content_name_request_list[counter1][counter2] < current_minimum){
								temp_peer_index = counter1;
								temp_content_index = counter2;
								 current_minimum = content_name_request_list[counter1][counter2];								
							}
						}		
					}
				}

				if (flag){
					spdu.type = 'S';
					strncpy(spdu.data, peer_names[temp_peer_index], 10);
					strncpy(spdu.data + 10, content_names[temp_peer_index][temp_content_index], 10);
					strncpy(spdu.data + 20, addresses[temp_peer_index][temp_content_index], 80);
					printf("\n\tContent found with peer: %s at address: %s!", peer_names[temp_peer_index], addresses[temp_peer_index][temp_content_index]);
					content_name_request_list[temp_peer_index][temp_content_index]++;
					//printf("%s %s %s", peer_names[temp_peer_index], content_names[temp_peer_index][temp_content_index], addresses[temp_peer_index]);
				}else{
					spdu.type = 'E';
					strncpy(spdu.data, "Error: Content not found", 100);
					printf("\n\tContent not found!!");
				}
				(void) sendto(s, &spdu, sizeof(spdu), 0, (struct sockaddr *)&fsin, sizeof(fsin));

				break;
			}
			case 'T': /* De-Register */{
				printf("");

				int temp_peer_index = 0; /* Temporary index for peer */
				int temp_content_index = 0;

				int flag  = 0; /* Flag: peer exists? */
				int flag2 = 0; /* Flag: content exists? */

				char peer_name[11]; /* Extract peer name */
				strncpy(peer_name, rpdu.data, 10);
				peer_name[10] = '\0';

				char content_name[11]; /* Extract content name */
				strncpy(content_name, rpdu.data + 10, 10);
				content_name[10] = '\0';

				printf("\n\n\nDeRegistration Request: \n\tPeer: %s \n\tContent: %s", peer_name, content_name);
				
				/* Check if peer already exists */
				int i = 0;
				for (i; i<peer_names_index; i++){
					if (strcmp(peer_names[i], peer_name) == 0){
						temp_peer_index = i;
						flag = 1; /* Peer found */
						printf("\n\tPeer found!");
						break;
					}
				}

				if (!flag){ /* peer doesnt even exist */
					char msg[100];
					snprintf(msg, sizeof(msg), "Error: Peer %s doesnt exist:\n", peer_name);
					struct pdu spdu;
					spdu.type = 'E';
					strncpy(spdu.data, msg, sizeof(spdu.data));
					(void) sendto(s, &spdu, sizeof(spdu), 0, (struct sockaddr *)&fsin, sizeof(fsin));
					printf("\n\tPeer not found!");
				}else{ /* Existing peer - check for existing content */
					i = 0;
					for (i; i<content_names_index[temp_peer_index]; i++){
						if (strcmp(content_names[temp_peer_index][i], content_name) == 0){
							temp_content_index = i;
							flag2 = 1; /* content found */
							printf("\n\tContent found!");
							break;
						}
					}
					if (flag2){ /* deregister content */
						i = temp_content_index;
						for (i; i<(content_names_index[temp_peer_index]-1); i++){
							strcpy(content_names[temp_peer_index][i], content_names[temp_peer_index][i + 1]);
						}
		fflush(stdout);				content_names_index[temp_peer_index]--;
						struct pdu spdu;
						spdu.type = 'A';
						spdu.data[0] = '\0';
						(void) sendto(s, &spdu, sizeof(spdu), 0, (struct sockaddr *)&fsin, sizeof(fsin));
						printf("\n\t\tContent DeRegistered Successfully!");
					}else{ 
						char errorMsg[100];
						snprintf(errorMsg, sizeof(errorMsg),
							"Error: Peer has no content %s\n", content_name);
						struct pdu spdu;
						spdu.type = 'E';
						strncpy(spdu.data, errorMsg, sizeof(spdu.data));
						(void) sendto(s, &spdu, sizeof(spdu), 0, (struct sockaddr *)&fsin, sizeof(fsin));
						printf("\n\tContent not found!");
					}
				}
fflush(stdout);
				break;
			}
			case 'O': /* List Content */{
				printf("\n\n\nList Request!");
				int counter1=0;
				int counter2=0;
				struct pdu spdu;
				spdu.type = 'O';				
				
				for (counter1; counter1 < peer_names_index; counter1++){
					for (counter2; counter2 < content_names_index[counter1]; counter2++){
						strncpy(spdu.data, content_names[counter1][counter2], sizeof(spdu.data));
						(void) sendto(s, &spdu, sizeof(spdu), 0, (struct sockaddr *)&fsin, sizeof(fsin));
					}
				}

				spdu.type = 'F';
				(void) sendto(s, &spdu, sizeof(spdu), 0, (struct sockaddr *)&fsin, sizeof(fsin));
				
				break;
			}
			case 'E': {/* Error case */
				printf("\n\n\nError Received!");
				break;}
			default: {/* Invalid PDU type */
				fprintf(stderr, "Received rpdu.type: %s [port]\n", rpdu.type);
				exit(1);
			}
		}
		

	}
	
}

