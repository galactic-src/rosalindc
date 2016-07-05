#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 

    char sendBuff[1025];
    time_t ticks; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 


   int keepalive_true = 1;
   int optlen = sizeof keepalive_true;
   if (setsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE, &keepalive_true, optlen) < 0)
   {
      printf("Error setting socket to keep alive: %s\n", strerror(errno));
      exit(1);
   }
   if (getsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE, &keepalive_true, &optlen) < 0)
   {
      printf("Error getting socket keepalive value: %s\n", strerror(errno));
      exit(1);
   }
   printf("After setting, keepalive value is %d\n", keepalive_true);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 

    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

        

        ticks = time(NULL);
        snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
        write(connfd, sendBuff, strlen(sendBuff)); 

        close(connfd);
        sleep(1);
     }
}
