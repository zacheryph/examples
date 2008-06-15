#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/uio.h>

int main (void)
{
	int len, io, ret, bytes;
	char buf[513];

	io = fileno(stdin);

	for (;;) {
		ret = ioctl(io, FIONREAD, &bytes);
		if (-1 == ret) printf("ret:%d: %s\n", ret, strerror(errno));
		if (0 < bytes) {
			len = read(io, buf, 512);
			buf[len] = '\0';
			printf (">> %s", buf);
		}
		sleep(1);
	}
}
