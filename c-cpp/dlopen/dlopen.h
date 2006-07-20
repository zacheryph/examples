#include <string>

typedef void (*Function)();

extern "C" void init_mod ();
bool reg_function(const std::string &, Function);
