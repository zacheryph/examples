#include <string>

typedef void (*Function) ();
typedef bool (*RegFunc) (const std::string &, Function);
typedef void (*ModInit) (RegFunc);

extern "C" void init_mod (RegFunc);
bool reg_function(const std::string &, Function);
