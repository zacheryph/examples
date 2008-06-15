#include <stdio.h>
#include <event.h>
#include <errno.h>

#define BUFLEN 256

void stdin_read (int fd, short event, void *arg)
{
	int len;
	char buf[BUFLEN+1];
	struct event *ev = arg;

	event_add(ev, NULL);

	len = read(fd, buf, BUFLEN);
	if (len == -1) {
		perror("read");
		return;
	} else if (len == 0) {
		fprintf(stderr, "Connection closed\n");
		return;
	}

	buf[len] = '\0';
	printf(">> %s\n", buf);
}

int main (void)
{
	int ret, io_fd;
	struct event ev;

	event_init();
	io_fd = dup(fileno(stdin));

	printf("method:%s new_stdin:%d\n", event_get_method(), io_fd);

	event_set(&ev, io_fd, EV_READ, stdin_read, &ev);
	printf("event_add:%d\n", event_add(&ev, NULL));

	ret = event_dispatch();
	printf("error: %d errno:%d\nerror: %s\n", ret, errno, strerror(errno));
}
