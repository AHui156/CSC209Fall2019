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
	int fd_store[MAXDEV] = {0};	

	while(1) {		
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
			printf("Waiting for Sensors update...\n");
			continue;
		} 

		// new connection requested
		if (FD_ISSET(gatewayfd, &copy_fds)){
			peerfd = accept_connection(gatewayfd);
			if(peerfd != -1){
				// add peerfd to the fd_set
				FD_SET(peerfd, &all_fds); 

				// store peerfd for later checking
				for (int i = 0; i < MAXDEV; i++){
					// Find the first 'empty' fd_store slot and store peerfd
					if (fd_store[i] == 0){
						fd_store[i] = peerfd;
						break; 
					}
				}

				// update max_fd 
				max_fd = MAXFD(peerfd, max_fd);
				continue;
			}
		}

		// traverse fd_store and process all messages 
		for(int i = 0; i < MAXDEV; i++){
			// printf("Checking fd_store[%d] now\n", i);
			if(fd_store[i] == 0){ continue; }

			// Check if peerfd has sent something
			if(FD_ISSET(fd_store[i], &copy_fds)){

				// read from client, process_message, write to client
				if (read(fd_store[i], cig_serialized, CIGLEN) != CIGLEN){
					perror("read");
				} else {
					printf("RAW MESSAGE: %s\n", cig_serialized);

					// unpack message to cignal 
					unpack_cignal(cig_serialized, &cig); 
					
					// process message and alter cignal
					if (process_message(&cig, device_record) == -1) { continue; }
					
					//send message back to client 
					cig_serialized = serialize_cignal(cig); 

					if (write(fd_store[i], serialize_cignal(cig), CIGLEN) != CIGLEN){
						perror("write"); 
					}					
				}
				close(fd_store[i]);
			}

			// processing of client is complete 
			// remove from the fd_set and fd_store 
			FD_CLR(fd_store[i], &all_fds); 
			fd_store[i] = 0;
		}
	}
	return 0;
}
