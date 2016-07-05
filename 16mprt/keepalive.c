

int main(void)
{
   int tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
   if (tcpSocket < 0)
   {
      printf("Error opening socket\n");
      exit(1);
   }
   // set it to use keepalive
   int keepalive_true = 1;
   int optlen = sizeof keepalive_true;
   if (setsockopt(tcpSocket, SOL_SOCKET, SO_KEEPALIVE, &keepalive_true, optlen) < 0)
   {
      printf("Error setting socket to keep alive: %s\n", strerror(errno));
      exit(1);
   }
   if (getsockopt(tcpSocket, SOL_SOCKET, SO_KEEPALIVE, &keepalive_true, &optlen) < 0)
   {
      printf("Error getting socket keepalive value: %s\n", strerror(errno));
      exit(1);
   }
   printf("After setting, keepalive value is %d\n", keepalive_true);


}
