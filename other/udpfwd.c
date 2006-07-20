
//
// udp port forwarder (good for bypassing the shitty iSU dorm lag) -- vulture
//
// usage: log in to vincent (telnet isua.iastate.edu)
//        compile (add gcc; gcc udpfwd.c -o udpfwd
//        run (udpfwd isuaX.iastate.edu:port targetserver:port)
//        connect to proxy in cs/q3/etc. on appropriate machine/port
//
// you can telnet into isua3.iastate.edu (or any number 1-5) if you want to be
// sure of your machine
//

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#define closesocket(a) close(a)
#define SOCKET int

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/timeb.h>

int main(int argc, char**argv)
{
//  struct WSAData wsadata;
  SOCKET bncsock,dstsock,clientsock,authsock1,authsock2,authsock3;
  fd_set rdfs;
  struct hostent *he;
  char bnc[256],dst[256];
  int bncport=0,dstport=0;
  struct sockaddr_in bncsa,dstsa,clientsa,lastsa,lastsaA,lastsaB,lastsaC;
  struct sockaddr_in dstsaA,dstsaB,dstsaC,clientsaA,clientsaB,clientsaC;
  struct timeval tv;
  char buf[32768];
  int ret,newret,dummy;
  struct timeb now;
  int delta;
  struct ll* cur;
  int inburst,didburst;
//  int DELAY;

  printf("udpfwd / vulture a.k.a. Sean Stanek <vulture@iastate.edu>\n");
  if(argc != 3) {
    printf("usage: %s [bnc:]port dst:port\n",argv[0]);
    return 0;
  }

//  WSAStartup(0x0002,&wsadata);

  bncsock = socket( AF_INET, SOCK_DGRAM, IPPROTO_IP );

  bncport = bnc[0] = 0;
  if(strchr(argv[1],':')) {
    sscanf(argv[1],"%[^:]:%d",bnc,&bncport);
  } else {
    sscanf(argv[1],"%d",&bncport);
    strcpy(bnc,"localhost");
  }

  dstport = dst[0] = 0;
  sscanf(argv[2],"%[^:]:%d",dst,&dstport);

  if(!bncport || !dstport) {
    printf("\nerror: cannot use port 0, specify a (different) port [%d / %d]\n",bncport,dstport);
    return 3;
  }

  he = gethostbyname(bnc);
  if(!he) {
    printf("\nerror: couldn't resolve '%s'\n",bnc);
    return 2;
  }
  bncsa.sin_family = AF_INET;
  memcpy(&bncsa.sin_addr.s_addr, he->h_addr, he->h_length);
  bncsa.sin_port = htons(bncport);
  if(bind(bncsock,(struct sockaddr*) &bncsa,sizeof(bncsa))) {
    printf("\nerror: couldn't bind to %s:%d\n",bnc,bncport);
    return 1;
  }

  he = gethostbyname(dst);
  if(!he) {
    printf("\nerror: couldn't resolve '%s'\n",dst);
    return 2;
  }
  dstsa.sin_family = AF_INET;
  memcpy(&dstsa.sin_addr.s_addr, he->h_addr, he->h_length);
  dstsa.sin_port = htons(dstport);

  printf("\nbouncing %s:%d <=> %s:%d\n\n",bnc,bncport,dst,dstport);


  while(1) {

    // newret = sendto(dstsock, head->next->buf, head->next->buflen, 0, 0, 0);

    FD_ZERO(&rdfs);
    FD_SET(bncsock,&rdfs);

    tv.tv_sec = 5;
    tv.tv_usec = 0;
    if(select(1024, &rdfs, 0, 0, &tv)) {

      if(FD_ISSET(bncsock,&rdfs)) {
        dummy = sizeof(clientsa);
        ret = recvfrom(bncsock, buf, 16384, 0, (struct sockaddr*)&clientsa, &dummy);
        dummy = sizeof(clientsa);
        if(clientsa.sin_addr.s_addr == dstsa.sin_addr.s_addr) {
          sendto(bncsock, buf, ret, 0, (struct sockaddr*)&lastsa, dummy);
        } else {
          sendto(bncsock, buf, ret, 0, (struct sockaddr*)&dstsa, dummy);
          memcpy(&lastsa, &clientsa, sizeof(lastsa));
        }
      }

    }
  }

//  WSACleanup();
}
