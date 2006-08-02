/**
 * example embedded Tcl project
 * hopefully its nicer to the touch then Lua :-)
 */

#include <iostream>
#include <string>

#include "tcl.h"

static int tcl_mycout (ClientData, Tcl_Interp*, int, const char**);

int main (int argc, char *argv[])
{
	/* initialize Tcl Interpreter */
	Tcl_Interp *interp;
	interp = Tcl_CreateInterp ();
	Tcl_Init (interp);

	/* Set example variable and insert command */
	char *mycvar = "this is my cvar";
	Tcl_CreateCommand (interp, "mycout", tcl_mycout, 0, 0);
	Tcl_SetVar (interp, "mycvar", mycvar, TCL_GLOBAL_ONLY);

	/* exec example.tcl */
	std::cout << "Evaluating: example.tcl\n";
	Tcl_EvalFile (interp, "./example.tcl");

	/* you should see output from mycout now */

	/* now read mytclvar from the script */
	std::string mytclvar;
	mytclvar = Tcl_GetVar (interp, "mytclvar", TCL_GLOBAL_ONLY);
	std::cout << "mytclvar = " << mytclvar << "\n";

	/* now try to execute mytclproc */
	char *params = " see me";
	int x;
	x = Tcl_VarEval (interp, "mytclproc", params, 0);
	if (x == TCL_ERROR)
		std::cerr << "Tcl Error: " << interp->result << "\n";
	else
		std::cout << "Tcl Said: " << interp->result << "\n";

	return 0;
}

static int tcl_mycout (ClientData cd, Tcl_Interp *tcl, int argc, const char **argv)
{
	std::cout << "tcl_mycout: " << argv[1] << "\n";
	return TCL_OK;
}

/* Servey Says! Tcl > Lua by far */
