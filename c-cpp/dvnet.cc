#include <iostream>
#include <string>

#include "dvnet/socket.h"

int main (int argc, char *argv[])
{
	Dv::Net::Socket sock ("127.0.0.1", 80);

	if (sock)
	{
		sock << "GET / HTTP/1.0\r\n\r\n" << std::flush;
		std::cout << "Sent HTTP Request...\n\n";
	}
	
	std::string line;
	while (!sock.error ())
	{
		getline (sock, line);
		std::cout << line << "\n";
	}

	if (sock.error ())
		std::cout << "ERROR: " << sock.strerror () << "\n";

	return 0;
}
