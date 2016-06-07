// This code implementation is the intellectual property of
// the RD44 GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: G4OpenGLXView.hh,v 2.5 1998/10/29 09:37:23 barrand Exp $
// GEANT4 tag $Name: geant4-00 $
//
// 
// Andrew Walkden  7th February 1997
// G4OpenGLXView : Class to provide XWindows specific
//                 functionality for OpenGL in GEANT4

#ifdef G4VIS_BUILD_OPENGLX_DRIVER

#ifndef G4OPENGLXVIEW_HH
#define G4OPENGLXVIEW_HH

#include "G4VView.hh"
#include "G4OpenGLScene.hh"
#include "globals.hh"
#include <rw/tvordvec.h>

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/Xmu/StdCmap.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

class G4OpenGLScene;

class G4OpenGLXView: virtual public G4OpenGLView {

public:
  G4OpenGLXView (G4OpenGLScene& scene);
  ~G4OpenGLXView ();
  void FinishView ();
  void print();

protected:
  void GetXConnection ();
  void CreateGLXContext (XVisualInfo* vi);
  virtual void CreateMainWindow ();

  char                              print_string[50];
  G4bool                            print_colour,
                                    vectored_ps;

//////////////////////////////Vectored PostScript production functions//////////////////////////////
  void printBuffer(GLint, GLfloat*);
  GLfloat* spewPrimitiveEPS (FILE*, GLfloat*);
  void spewSortedFeedback (FILE*, GLint, GLfloat*);
  void spewWireframeEPS (FILE*, GLint, GLfloat*, char*);
  void print3DcolorVertex(GLint, GLint*, GLfloat*);
  G4float                           pointSize;


//////////////////////////////Pixmap (screen dump) production functions//////////////////////////////
  GLubyte* grabPixels (int inColor,
		       unsigned int width,
		       unsigned int height);
  int generateEPS (char* filnam,
		   int inColour,
		   unsigned int width,
		   unsigned int height);
  GLXContext create_GL_print_context (XVisualInfo*& pvi, G4bool& db);

  XWindowAttributes                 xwa;
  Display                           *dpy;
  XVisualInfo                       *vi_immediate,
                                    *vi_stored,
                                    *vi;
  Colormap                          cmap;
  XSetWindowAttributes              swa;
  GLXDrawable                       win;
  GLXContext                        cx;
  XEvent                            event;
  G4int                             *attributeList,
                                    errorBase,
                                    eventBase,
                                    major,
                                    minor,
                                    x_origin,
                                    y_origin;
  XSizeHints                        *norm_hints;
  XWMHints                          *wm_hints;
  XClassHint                        *class_hints;
  Pixmap                            icon_pixmap;
  XSizeHints                        *size_hints;
  G4int                             WinSize_x,
                                    WinSize_y;
  Atom                              Xatom;
  XTextProperty                     windowName,
                                    iconName;
  char                              charViewName [100];

};

static const char* gouraudtriangleEPS[] =
{
  "/bd{bind def}bind def /triangle { aload pop   setrgbcolor  aload pop 5 3",
  "roll 4 2 roll 3 2 roll exch moveto lineto lineto closepath fill } bd",
  "/computediff1 { 2 copy sub abs threshold ge {pop pop pop true} { exch 2",
  "index sub abs threshold ge { pop pop true} { sub abs threshold ge } ifelse",
  "} ifelse } bd /computediff3 { 3 copy 0 get 3 1 roll 0 get 3 1 roll 0 get",
  "computediff1 {true} { 3 copy 1 get 3 1 roll 1 get 3 1 roll 1 get",
  "computediff1 {true} { 3 copy 2 get 3 1 roll  2 get 3 1 roll 2 get",
  "computediff1 } ifelse } ifelse } bd /middlecolor { aload pop 4 -1 roll",
  "aload pop 4 -1 roll add 2 div 5 1 roll 3 -1 roll add 2 div 3 1 roll add 2",
  "div 3 1 roll exch 3 array astore } bd /gouraudtriangle { computediff3 { 4",
  "-1 roll aload 7 1 roll 6 -1 roll pop 3 -1 roll pop add 2 div 3 1 roll add",
  "2 div exch 3 -1 roll aload 7 1 roll exch pop 4 -1 roll pop add 2 div 3 1",
  "roll add 2 div exch 3 -1 roll aload 7 1 roll pop 3 -1 roll pop add 2 div 3",
  "1 roll add 2 div exch 7 3 roll 10 -3 roll dup 3 index middlecolor 4 1 roll",
  "2 copy middlecolor 4 1 roll 3 copy pop middlecolor 4 1 roll 13 -1 roll",
  "aload pop 17 index 6 index 15 index 19 index 6 index 17 index 6 array",
  "astore 10 index 10 index 14 index gouraudtriangle 17 index 5 index 17",
  "index 19 index 5 index 19 index 6 array astore 10 index 9 index 13 index",
  "gouraudtriangle 13 index 16 index 5 index 15 index 18 index 5 index 6",
  "array astore 12 index 12 index 9 index gouraudtriangle 17 index 16 index",
  "15 index 19 index 18 index 17 index 6 array astore 10 index 12 index 14",
  "index gouraudtriangle 18 {pop} repeat } { aload pop 5 3 roll aload pop 7 3",
  "roll aload pop 9 3 roll 4 index 6 index 4 index add add 3 div 10 1 roll 7",
  "index 5 index 3 index add add 3 div 10 1 roll 6 index 4 index 2 index add",
  "add 3 div 10 1 roll 9 {pop} repeat 3 array astore triangle } ifelse } bd",
  NULL
};

typedef struct _Feedback3Dcolor {
  GLfloat x;
  GLfloat y;
  GLfloat z;
  GLfloat red;
  GLfloat green;
  GLfloat blue;
  GLfloat alpha;
} Feedback3Dcolor;

#endif

#endif