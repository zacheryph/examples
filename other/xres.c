#include <stdlib.h>
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xmu/StdSel.h>
#include <X11/Xmd.h>
#include <X11/extensions/xf86vmode.h>

//=======================================
// Prototypes...
//---------------------------------------
Display* XOpen(char name[256]);

//=======================================
// Globals...
//---------------------------------------
int MajorVersion, MinorVersion;
int EventBase, ErrorBase;

int main(void) {
 XF86VidModeModeLine mode_line;
 int dot_clock;
 Display* dpy;

 // open display
 dpy = XOpen("null");

 // query modeline
 if (!XF86VidModeGetModeLine (dpy,DefaultScreen(dpy),&dot_clock,&mode_line)) {
  fprintf(stderr,"XF86VidModeGetModeLine: could not get modeline timings\n");
  XCloseDisplay(dpy);
  exit(1);
 }

 printf("%dx%d\n",mode_line.hdisplay,mode_line.vdisplay);
 XCloseDisplay(dpy);
 exit(0);
}

//=======================================
// open display, prepare vidmode ext...
//---------------------------------------
Display* XOpen(char name[256]) {
 Display *dpy;
 char display[256] = "";

 if (strcmp(name,"null") == 0) {
  strcpy(display,getenv("DISPLAY"));
 } else {
  strcpy(display,name);
 }
 if(!(dpy = XOpenDisplay(display))) {
  fprintf (stderr, "unable to open display: %s\n",getenv("DISPLAY"));
  return(NULL);
 }
 if (!XF86VidModeQueryVersion(dpy, &MajorVersion, &MinorVersion)) {
  fprintf (stderr, "Unable to query video extension version\n");
  return(NULL);
 }
 if (!XF86VidModeQueryExtension(dpy, &EventBase, &ErrorBase)) {
  fprintf (stderr, "Unable to query video extension information\n");
  return(NULL);
 }
 return(dpy);
}


