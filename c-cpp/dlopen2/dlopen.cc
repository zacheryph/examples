
#include <iostream>
#include <string>

#include <dlfcn.h>

#include "dlopen.h"

Function dynlink_func;

int main (int argc, char *argv[])
{
	std::cout << " Binary: Attempting to Load Module\n";

	void *hand = dlopen ("./dlopen_lib.so", RTLD_LAZY);
	if (!hand)
	{
		std::cerr << " Binary: ERROR: Unable to Load ./dlopen_lib.so\n";
		std::cerr << " Binary: DLERROR: " << dlerror () << "\n";
		return 1;
	}

	std::cout << " Binary: Attempting to exec init_mod () in Library\n";

	ModInit m_init = (ModInit) dlsym (hand, "init_mod");
	if (!m_init)
	{
		std::cerr << " Binary: ERROR: Unable to Load init_mod ();\n";
		std::cerr << " Binary: DLERROR: " << dlerror () << "\n";
		return 1;
	}

	m_init (reg_function);
	
	std::cout << " Binary: Attempting to exec Function Pointer in Library\n";

	dynlink_func ();

	dlclose(hand);
	return 0;
}

bool reg_function (const std::string &name, Function funct)
{
	dynlink_func = funct;
	return 1;
}
