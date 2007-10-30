/* pm.c
 * author: OrngeTide ! #c @ efnet
 * start: Sun Oct 28 05:32:51 PDT 2007
 * end: Sun Oct 28 06:41:04 PDT 2007
 * */
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/***********************************************************
 * macros
 ***********************************************************/
#define NR(x) (sizeof(x)/sizeof*(x))
#ifdef NDEBUG
#define FAILON(e,reason) if(e) { perror("FAILED:" reason); return 0; }
#else
#define FAILON(e,reason) if(e) { perror(#e ":FAILED:" reason); return 0; }
#endif
#define STATIC static
/***********************************************************
 * client abstraction
 ***********************************************************/
#define CB_ARGS struct client *cl
struct client {
	int fd;
	void (*cb)(CB_ARGS);
	void *p;
	struct client **prev, *next;
};
static struct client *head;

STATIC struct client *clientadd(int fd, void (*cb)(CB_ARGS), void *p) {
	struct client *new;
	new=calloc(1, sizeof *new);
	FAILON(!new, "allocating client");
	new->fd=fd;
	new->cb=cb;
	new->p=p;
	new->prev=&head;
	new->next=head;
	if(head) head->prev=&new->next;
	head=new;
	return new;
}

/***********************************************************
 * socket abstraction
 ***********************************************************/
#ifdef __linux__
/***********************************************************
 * epoll() implementation
 ***********************************************************/
#include <sys/epoll.h>
#include <fcntl.h>
static int epfd;

STATIC void sockinit() {
	epfd=epoll_create(64);
}

STATIC void sockdone() {
	close(epfd);
}

STATIC int sockdel(int fd) {
	struct epoll_event ev;
	FAILON(epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev), "removing socket");
	return 1;
}

STATIC int sockadd(int fd, void (*cb)(CB_ARGS), void *p) {
	struct epoll_event ev;
	FAILON(fcntl(fd, F_SETFL, O_NONBLOCK), "setting socket to non-blocking");
	ev.events=EPOLLIN|EPOLLET;
	ev.data.ptr=clientadd(fd, cb, p);
	if(!ev.data.ptr) return 0;
	/* BUG: should remove added client on failure */
	FAILON(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev), "adding socket");
	return 1;
}

STATIC int sockpoll(int msec) {
	struct epoll_event ev[8];
	int res;
	struct client *cl;
	res=epoll_wait(epfd, ev, NR(ev), msec);
	FAILON(res<0, "while polling sockets");
	while(res) {
		res--;
		cl=ev[res].data.ptr;
		cl->cb(cl);
	}
	return 1;
}

#else
#error Unsupported platform
#endif

/***********************************************************
 * TCP/IP
 ***********************************************************/
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

static void client_cb(CB_ARGS) {
	char buf[512];
	int res;
	while(1) {
		res=read(cl->fd, buf, sizeof buf);
		if(res<0) {
			if(errno!=EAGAIN) {
				perror("reading from socket");
			}
			return;
		} else if(!res) {
			sockdel(cl->fd);
			/* TODO: remove the client */
			return;
		}
		printf("Data!\n");
	}
}

static void server_cb(CB_ARGS) {
	struct sockaddr_in sin;
	socklen_t len;
	int new;
	len=sizeof sin;
	while((new=accept(cl->fd, (struct sockaddr*)&sin, &len))>=0) {
		printf("Connect!\n");
		if(!sockadd(new, client_cb, 0)) return;
	}
	if(errno!=EAGAIN) {
		perror("accept");
	}
}

STATIC int ip4add(unsigned short port) {
	struct sockaddr_in sin;
	int s;
	s=socket(PF_INET, SOCK_STREAM, 0);
	FAILON(s<0, "creating listening socket");
	memset(&sin, 0, sizeof sin);
	sin.sin_port=htons(port);
	FAILON(bind(s, (struct sockaddr*)&sin, sizeof sin), "binding to port");
	FAILON(listen(s, 10), "enabling listening socket");
	sockadd(s, server_cb, 0);
	return 1;
}

/***********************************************************
 * TCP/IP
 ***********************************************************/
int main() {
	sockinit();
	ip4add(4444);
	while(sockpoll(10000)) printf("tick\n");
	sockdone();
	return 0;
}
