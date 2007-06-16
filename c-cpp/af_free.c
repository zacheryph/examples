#include <err.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main (int argc, char **argv)
{
  int salen, ret;
  struct sockaddr *sa;
  struct addrinfo hints;
  struct addrinfo *res;

  char host[NI_MAXHOST];
  char port[NI_MAXSERV];

  memset (&hints, '\0', sizeof(hints));
  memset (host, '\0', NI_MAXHOST); memset (port, '\0', NI_MAXSERV);
  hints.ai_family = PF_UNSPEC;
  hints.ai_flags = AI_NUMERICHOST;

  getaddrinfo ("2001:19f0::dead:beef:cafe", "6667", &hints, &res);
  if (!res) errx (0, "getaddrinfo() failed");
  while (res) {
    printf ("getnameinfo...\n");
    sa = res->ai_addr;
    salen = res->ai_addrlen;
    memset (host, '\0', NI_MAXHOST); memset (port, '\0', NI_MAXSERV);
    ret = getnameinfo (sa, salen, host, NI_MAXHOST, port, NI_MAXSERV, 0);
    printf ("    ret:%d host:%s  port:%s\n", ret, host, port);
    memset (host, '\0', NI_MAXHOST); memset (port, '\0', NI_MAXSERV);
    ret = getnameinfo (sa, salen, host, NI_MAXHOST, port, NI_MAXSERV, NI_NUMERICHOST);
    printf ("    ret:%d host:%s  port:%s\n", ret, host, port);
    memset (host, '\0', NI_MAXHOST); memset (port, '\0', NI_MAXSERV);
    ret = getnameinfo (sa, salen, host, NI_MAXHOST, port, NI_MAXSERV, NI_NUMERICSERV);
    printf ("    ret:%d host:%s  port:%s\n", ret, host, port);
    memset (host, '\0', NI_MAXHOST); memset (port, '\0', NI_MAXSERV);
    ret = getnameinfo (sa, salen, host, NI_MAXHOST, port, NI_MAXSERV, (NI_NUMERICHOST|NI_NUMERICSERV));
    printf ("    ret:%d host:%s  port:%s\n\n", ret, host, port);
    res = res->ai_next;
  }
  freeaddrinfo (res);
  return 0;
}

/*
user68:~/src/examples/c-cpp context$ gcc -o af af_free.c ; ./af
getnameinfo...
    host:k  port:l3-hawk
    host:k  port:l3-hawk
    host:irc.choopa.ca  port:2842
    host:64.237.34.150  port:6667
*/
