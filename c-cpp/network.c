// quick network methods wrote for an app.
// was gonna add a lot more, but abusing eggdrop 2.0's libeggdrop
// was a lot easier, and seemed more enjoyable at the time.
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>

static fd_set fds;
static struct timeval wait;

/* net_init(): initialize networking
 * returns: -1:fail 0:success
 */
int net_init ()
{
	wait.tv_sec = 1;
	wait.tv_usec = 0;
	FD_ZERO (&fds);
	return 0;
}

/* sock_open("ip.ad.dr.ess", port)
 * returns: -1:fail +:socket fd (success)
 * note: close the fd when your done with it!
 */
int net_connect (const char *ipaddr, unsigned int port)
{
	int sockfd;
	struct sockaddr_in raddr;

	if (-1 == (sockfd = socket(PF_INET, SOCK_STREAM, 0)))
		return -1;

	if (0 == inet_aton(ipaddr, &(raddr.sin_addr))) {
		close (sockfd);
		return -1;
	}

	memset(&(raddr.sin_zero), '\0', 8);
	raddr.sin_family = AF_INET;
	raddr.sin_port = htons(port);
	
	if (-1 == connect(sockfd, (struct sockaddr *) &raddr, sizeof (struct sockaddr))) {
		close (sockfd);
		return -1;
	}

	// yey we connected
	FD_SET(sockfd, &fds);
	return sockfd;
}

int net_listen (const char *ipaddr, unsigned int port)
{
	int sockfd;
	struct sockaddr_in laddr;

	memset(&(laddr.sin_zero), '\0', 8);
	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(4363);
	laddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (-1 == (sockfd = socket(PF_INET, SOCK_STREAM, 0)))
		return -1;

	if (-1 == bind(sockfd, (struct sockaddr *) &laddr, sizeof(struct sockaddr))
			|| -1 == listen(sockfd, 3)) {
		close(sockfd);
		return -1;
	}

	return sockfd;
}
