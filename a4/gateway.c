#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <time.h>
#include "socket.h"  
#include "message.h"
#include "controller.h"

#define MAXFD(x,y) ((x) >= (y)) ? (x) : (y)

int main(int argc, char *argv[]){
	int port;
	struct cignal cig;
	// A buffer to store a serialized message
	char *cig_serialized = malloc(sizeof(char)*CIGLEN);
	// An array to registered sensor devices
	int device_record[MAXDEV] = {0};
	
	if(argc == 2){
		port = strtol(argv[1], NULL, 0);
	} else{
		fprintf(stderr, "Usage: %s port\n", argv[0]);
		exit(1);
	}

	int gatewayfd = set_up_server_socket(port);
	printf("\nThe Cignal Gateway is now started on port: %d\n\n", port);
	int peerfd;
	
	/* TODO: Implement the body of the server.  
	 *
	 * Use select so that the server process never blocks on any call except
	 * select. If no sensors connect and/or send messsages in a timespan of
	 * 5 seconds then select will return and print the message "Waiting for
	 * Sensors update..." and go back to waiting for an event.
	 * 
	 * The server will handle connections from devices, will read a message from
	 * a sensor, process the message (using process_message), write back
	 * a response message to the sensor client, and close the connection.
	 * After reading a message, your program must print the "RAW MESSAGE"
	 * message below, which shows the serialized message received from the *
	 * client.
	 * 
	 *  Print statements you must use:
     * 	printf("Waiting for Sensors update...\n");
	 * 	printf("RAW MESSAGE: %s\n", YOUR_VARIABLE);
	 */

	// Set up the fd_set 
	int max_fd = gatewayfd; 
	fd_set all_fds; 
	FD_ZERO(&all_fds);
	FD_SET(gatewayfd, &all_fds); 

	while(1) {
		//select with timeout of 5 sec. fd_set should contain all sensor fds
		// Problem: do we do manage incoming connection in a serialized fashion? 
		// No, all clients are responeded to per loop

		struct timeval time_out; 
		time_out.tv_sec = 5;
		time_out.tv_usec = 0; 
		
		// copy fd_set 
		fd_set copy_fds = all_fds; 
		int select_ret = select(max_fd + 1, &copy_fds, NULL, NULL, &time_out);
		if (select_ret == -1){
			perror("select");  
			exit(EXIT_FAILURE);
		} else if (select_ret == 0){
			// No fds are ready ... 0 means timeout? 
			printf("Waiting for Sensors update...\n");
			continue;
		} 

		if (FD_ISSET(gatewayfd, &all_fds)){
			// iterate through number of returns from select
			for (int i = 0; i < select_ret; i++){		
				peerfd = accept_connection(gatewayfd); 
				if (peerfd != -1){
					// connection succesful, read from fd
					if (read(peerfd, cig_serialized, CIGLEN) != CIGLEN){
						perror("read");
						continue;
					} else {
						printf("RAW MESSAGE: %s\n", cig_serialized);
						// unpack message to cignal 
						unpack_cignal(cig_serialized, &cig); 

						if (process_message(&cig, device_record) == -1) { 
							printf("Error in process_message");
							continue; 
						}
						//send message back to client 
						cig_serialized = serialize_cignal(cig); 

						if (write(peerfd, serialize_cignal(cig), CIGLEN) != CIGLEN){
							perror("write"); 
							continue; 
						}

						printf("********************END EVENT********************\n");

						close(peerfd);
					}
				}  
				
			}
		}		
	}
	return 0;
}
