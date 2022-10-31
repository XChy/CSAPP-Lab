#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int open_clientfd(char *hostname, char *port);

int main()
{
}

int open_clientfd(char *hostname, char *port)
{
      int clientfd;
      struct addrinfo hints, *listp, *p;

      memset(&hints, 0, sizeof(struct addrinfo));
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_flags = AI_NUMERICSERV;
      hints.ai_flags |= AI_ADDRCONFIG;
      if (getaddrinfo(hostname, port, &hints, &listp) != 0)
      {
            unix_error("Getaddrinfo error");
      }

      for (p = listp; p != NULL; p = p->ai_next)
      {
            if (clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
            {
                  continue;
            }

            if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1)
                  break;
            close(clientfd);
      }

      freeaddrinfo(listp);
      if (p == NULL)
      {
            return -1;
      }
      else
      {
            return clientfd;
      }
}