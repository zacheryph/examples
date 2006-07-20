
#include <iostream>
#include <string>

#include "dlopen.h"

void say_hello ()
{
	std::cout << "Library: say_hello () Function pointer execution\n";
	return;
}

extern "C" void init_mod ()
{
	std::cout << "Library: creating function pointer in binary\n";
	reg_function ("hello", say_hello);
	return;
}


