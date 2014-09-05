/* This page contains the client program. The following one contains the
 * server program. Once the server has been compiled and started, clients
 * anywhere on the Internet can send commands (file names) to the server.
 * The server responds by opening and returning the entire file requested.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>                     /* add by Kun*/
#include <stdlib.h>                     /* add by Kun*/
#include <stdio.h>                      /* add by Kun*/

#define SERVER_PORT 12345               /* arbitrary, but client and server must agree */
#define BUF_SIZE 4096                   /* block transfer size */

int main(int argc, char **argv)
{
    int c, s, bytes, fbl = 0;
    char buf[BUF_SIZE];                   /* buffer for incoming file */
    char filename[30];                    /* file name */
    struct hostent *h;                    /* info about server */
    struct sockaddr_in channel;           /* holds IP address */


    if (argc != 3){
          fatal("Usage: client server-name file-name\n");
    }
    strcpy(filename, argv[2]);
  
    if (!(h = gethostbyname(argv[1]))){
          fatal("gethostbyname failed\n");
    }else{
          printf("host address is %s\n", argv[1]);
    }


    //socket
    if ((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
          fatal("socket build error\n");
    }else{
          printf("socket build successfully\n");
    }

    memset(&channel, 0, sizeof(channel));
    channel.sin_family= AF_INET;
    memcpy(&channel.sin_addr.s_addr, h->h_addr, h->h_length);
    channel.sin_port= htons(SERVER_PORT);


    //connect
    if ((c = connect(s, (struct sockaddr *) &channel, sizeof(channel))) < 0){
          fatal("connect failed\n");
    }else{
          printf("connected to server\n");
    }
  
    recv(s, buf, BUF_SIZE, 0);
    printf("%s", buf);     /* print message from server*/
    bzero(buf, BUF_SIZE);

    send(s, filename, 30, 0);
    recv(s, buf, 1, 0);


    if(strcmp(buf, "1") == 0){
          printf("File %s found on Server\n", filename);
    }else{
          printf("File %s not found on Server\n", filename);
          exit(0);
    }
  
  
    /* Connection is now established. Send file name including 0 byte at end. */
    FILE *f = fopen(filename, "wb+");
    if(NULL == f){
          fatal("File %s not found\n", filename);
    }else{
          printf("Preparing receiving file %s\n", filename);
          bzero(buf, BUF_SIZE);    /* make buf empty before use it*/
          bytes = 0;

          while(bytes = recv(s, buf, BUF_SIZE, 0)){
                  if(bytes < 0){
                          fatal("Receive Error!\n");
                  }

                  fbl = fwrite(buf, sizeof(char), bytes, f);
                  if(fbl < bytes){
                          fatal("Write File Error!\n");
                  }

                  bzero(buf, BUF_SIZE);
          }

          fclose(f);
          printf("File %s is received from server %s\n", filename, argv[1]);
    }

    close(c);   /* close connection*/
    close(s);   /* close socket */
    return 0;
}


fatal(char *string)
{
  printf("%s", string);
  exit(1);
}
