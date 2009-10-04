/**
 * example embedded Tcl project
 * hopefully its nicer to the touch then Lua :-)
 */

#include <stdio.h>

#include "tcl.h"

static int tcl_mycout (ClientData, Tcl_Interp*, int, const char**);

int main (int argc, char *argv[])
{
  /* some variables */
  char *mycvar, *params;
  const char *mytclvar;
  int x;

  /* initialize Tcl Interpreter */
  Tcl_Interp *interp;
  interp = Tcl_CreateInterp ();
  Tcl_Init (interp);

  /* Set example variable and insert command */
  mycvar = "this is my cvar";
  Tcl_CreateCommand (interp, "mycout", tcl_mycout, 0, 0);
  Tcl_SetVar (interp, "mycvar", mycvar, TCL_GLOBAL_ONLY);

  /* exec example.tcl */
  printf("Evaluating: example.tcl\n");
  Tcl_EvalFile (interp, "./example.tcl");

  /* you should see output from mycout now */

  /* now read mytclvar from the script */
  mytclvar = Tcl_GetVar (interp, "mytclvar", TCL_GLOBAL_ONLY);
  printf("mytclvar = %s\n", mytclvar);

  /* now try to execute mytclproc */
  params = " see me";
  x = Tcl_VarEval (interp, "mytclproc", params, 0);
  if (x == TCL_ERROR)
    printf("Tcl Error: %s\n", interp->result);
  else
    printf("Tcl Said: %s\n", interp->result);

  return 0;
}

static int tcl_mycout (ClientData cd, Tcl_Interp *tcl, int argc, const char **argv)
{
  printf("tcl_mycout: %s\n", argv[1]);
  return TCL_OK;
}
