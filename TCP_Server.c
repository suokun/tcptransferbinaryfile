#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>                     /* add by Kun*/
#include <stdio.h>                      /* add by Kun*/
#include <stdlib.h>                     /* add by Kun*/


#define SERVER_PORT 12345               /* arbitrary, but client and server must agree */
#define BUF_SIZE 4096                   /* block transfer size */
#define QUEUE_SIZE 10


int main(int argc, char *argv[])
{
    int s, b, l, sa, bytes, on = 1;
    char buf[BUF_SIZE];                   /* buffer for outgoing file */
    char filename[30];
    struct sockaddr_in channel;           /* hold's IP address */

    memset(&channel, 0, sizeof(channel)); /* zero channel */
    channel.sin_family = AF_INET;
    channel.sin_addr.s_addr = htonl(INADDR_ANY);
    channel.sin_port = htons(SERVER_PORT);

    if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
          fatal("socket creation failed\n");
    }else{
          printf("socket creation successful\n");
    }
  
  
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));

    //bind
    if ((b = bind(s, (struct sockaddr *) &channel, sizeof(channel))) < 0){
          fatal("bind failed\n");
    }else{
          printf("bind successfully\n");
    }

    //listen
    if ((l = listen(s, QUEUE_SIZE)) < 0){
          fatal("listen failed\n");
    }else{
          printf("listen successfully\n");
    }

    //accept
    while (1) {
        if ((sa = accept(s, 0, 0)) < 0){
                fatal("accept failed\n");
        }else{
                printf("accept successfully\n");
        }

        send(sa, "Welcome to TCP server\n", 22, 0);   /*send welcome message to client, added by Kun*/
        recv(sa, filename, 30, 0);

        //open file
        FILE *f = fopen(filename, "rb");
        if(NULL == f){
                send(sa, "0", 1, 0);
                printf("File %s not found\n", filename);
        }else{
                send(sa, "1", 1, 0);
                printf("Preparing sending file %s\n", filename);
                bzero(buf, BUF_SIZE);
                bytes = 0;
                while((bytes = fread(buf, sizeof(char), BUF_SIZE, f)) > 0){
                        printf("file block length is %d\n", bytes);
                        if(send(sa, buf, bytes, 0) < 0){
                                fatal("Send error\n");
                        }
                        bzero(buf, BUF_SIZE);
                }

                fclose(f);
                printf("Received file finished.\n");
        }

        close(sa);                               /* close connection */
    }

    close(s);
}


fatal(char *string)
{
    printf("%s", string);
    exit(1);
}
