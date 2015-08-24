
#include "view.h"
#include "object.h"
#include "defs.h"
#include "manager.h"
#include "log.h"
#include "luastate.h"
#include "typevar.h"
#include "input.h"
#include "component.h"
#include "glfun.h"

#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <GL/gl.h>

META_METHODS(View,
 METHOD(create, View::Create))
META_PROPERTY(View)
META_OBJECT(View, View, &Module::Instance)

unsigned int View :: messages_list[] = {MSG_FINISH, MSG_TEST_1, MSG_ADD_UNIT, 0};

//----------------------------------------------------------------

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;

View :: View(const char* Name) : Module(Name), winRect{400, 300, 0, 0}
 { ctx = 0;
   tabMessages = messages_list;
   do_update = (MUpdate) &View::init_update;
   metaClass = &Instance;
 }

bool View :: msg_processing()
 { int tMsg, param;
   Object* pobj;

   while(Manager::getMessage(getNumQueue(), tMsg, pobj, param))
    { switch(tMsg)
       { case MSG_FINISH:
          do_update = (MUpdate) &View::clear_update;
          return false;
         case MSG_TEST_1:
          LOG_SPAM("%s: Received a message MSG_TEST_1 (param %i)", getName(), param);
          break;
         case MSG_ADD_UNIT:
          addComp((Unit*)pobj);
          pobj->release();
          break;
       }
    }
   return true;
 }

void View :: set_var()
 { pool.emplace(vWIN_RECT, Link{1, setVec2i(winRect.width, winRect.height)}); }

bool View :: init(Lua::State &lua)
 { Lua::Var tab = lua[getName()];
   Lua::Var tmp;
   if(tab == Lua::TAB)
    { tmp = tab["width"];
      if(tmp == Lua::NUM) winRect.width = tmp;
      tmp = tab["height"];
      if(tmp == Lua::NUM) winRect.height = tmp;
      tmp = tab["pos_x"];
      if(tmp == Lua::NUM) winRect.posX = tmp;
      tmp = tab["pos_y"];
      if(tmp == Lua::NUM) winRect.posY = tmp;
    }
   LOG_INFO("%s: first init.", getName());
   return true;
 }

void View :: render()
 { for(auto &it : components)
    it.second->update();
   glXSwapBuffers(dpy, win);
 }

int View :: init_update(double tm)
 { int glx_major, glx_minor;
   int fbcount;

   dpy = XOpenDisplay(nullptr);
   if(dpy == nullptr)
    { LOG_ERROR("%s: Could not open display.", getName());
      do_update = &Module::empty_update;
      return 0;
    }
   // Get a matching FB config
   static int visual_attribs[] =
    {
      GLX_X_RENDERABLE    , True,
      GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
      GLX_RENDER_TYPE     , GLX_RGBA_BIT,
      GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
      GLX_RED_SIZE        , 8,
      GLX_GREEN_SIZE      , 8,
      GLX_BLUE_SIZE       , 8,
      GLX_ALPHA_SIZE      , 8,
      GLX_DEPTH_SIZE      , 24,
      GLX_STENCIL_SIZE    , 8,
      GLX_DOUBLEBUFFER    , True,
      //GLX_SAMPLE_BUFFERS  , 1,
      //GLX_SAMPLES         , 4,
      None
    };

   // FBConfigs were added in GLX version 1.3.
   if(!glXQueryVersion(dpy, &glx_major, &glx_minor) ||
       ((glx_major == 1) && (glx_minor < 3)) || (glx_major < 1))
    { LOG_ERROR("%s: Invalid GLX version.", getName());
      do_update = &Module::empty_update;
      return 0;
    }
   GLXFBConfig* fbc = glXChooseFBConfig(dpy, DefaultScreen(dpy), visual_attribs, &fbcount);
   if(!fbc)
    { LOG_ERROR( "%s: Failed to retrieve a framebuffer config.", getName());
      do_update = &Module::empty_update;
      return 0;
    }
   int best_fbc = -1, best_num_samp = -1;
   for (int i=0; i<fbcount; ++i)
    { XVisualInfo *vi = glXGetVisualFromFBConfig(dpy, fbc[i] );
      if ( vi )
       { int samp_buf, samples;
         glXGetFBConfigAttrib(dpy, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
         glXGetFBConfigAttrib(dpy, fbc[i], GLX_SAMPLES       , &samples  );
         if ((best_fbc < 0) || (samp_buf && (samples > best_num_samp)))
          best_fbc = i, best_num_samp = samples;
       }
      XFree( vi );
    }
   GLXFBConfig bestFbc = fbc[ best_fbc ];
   // Be sure to free the FBConfig list allocated by glXChooseFBConfig()
   XFree( fbc );
   // Get a visual
   XVisualInfo *vi = glXGetVisualFromFBConfig(dpy, bestFbc );

   XSetWindowAttributes swa;
   cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
   swa.colormap          = cmap;
   swa.background_pixmap = None ;
   swa.border_pixel      = 0;
   swa.event_mask        = StructureNotifyMask | FocusChangeMask | VisibilityChangeMask | ButtonPressMask; 

   win = XCreateWindow(dpy, RootWindow(dpy, vi->screen ),
                              0, 0, winRect.width, winRect.height,
                              0, vi->depth, InputOutput, vi->visual,
                              CWBorderPixel|CWColormap|CWEventMask, &swa );
  if (!win )
  {
    LOG_ERROR("Failed to create window.");
    do_update = &Module::empty_update;
    return 0;
  }

   // Done with the visual info data
   XFree( vi );

   wmDeleteMessage = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
   XStoreName(dpy, win, "GL 3.0 Window" );
   XMapWindow(dpy, win);
   XSetWMProtocols(dpy, win, &wmDeleteMessage, 1);
   XMoveWindow(dpy, win, winRect.posX, winRect.posY);

   glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
           glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );

   int context_attribs[] =
      {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 0,
        //GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        None
      };
   ctx = glXCreateContextAttribsARB(dpy, bestFbc, 0, True, context_attribs);
   XSync(dpy, False );

   Mousedevice::instance().setWindow(win);

   glXMakeCurrent(dpy, win, ctx);

   InitGL();

   glClearColor ( 0.05, 0.05, 0.05, 1 );
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
   glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
   glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

   set_var();
   LOG_INFO("%s: second init.", getName());
   Manager :: sendMessage(MSG_ACTIVE, nullptr, getNumQueue());
   do_update = (MUpdate) &View::run_update;
   return 1;
 }


int View :: run_update(double tm)
 { XEvent event;

   while(XPending(dpy)) /* loop to compress events */
    { XNextEvent(dpy, &event);
      switch (event.type)
       { case ClientMessage:
          if(event.xclient.data.l[0] == (int)wmDeleteMessage)
           { Manager :: sendMessage(MSG_FINISH, nullptr, 0);
             return 1;
           }
          break;
         case FocusIn:
          Mousedevice::instance().setFocus(true);
          Keyboard::instance().setFocus(true);
          break;
         case FocusOut:
          Mousedevice::instance().setFocus(false);
          Keyboard::instance().setFocus(false);
          break;
         case VisibilityNotify:
          LOG_SPAM("View: Visibility Notify");
          break;
         case UnmapNotify:
          LOG_SPAM("View: Visibility Unobscured");
          break;
         case ButtonPress:
          if(event.xbutton.button == Button4)
           { Mousedevice::instance().setWheelFlag(1);
             break;
           }
          if(event.xbutton.button == Button5)
           Mousedevice::instance().setWheelFlag(-1);
          break;
      }
    }

   if(msg_processing()) render();
   return 1;
 }

int View :: clear_update(double tm)
 { delComp();
   pool[vWIN_RECT].refCount --;
   del_var();
   glXMakeCurrent(dpy, 0, 0);
   glXDestroyContext(dpy, ctx);
   Keyboard::instance().setFocus(false);
   Mousedevice::instance().setWindow(0);
   XDestroyWindow(dpy, win);
   XFreeColormap(dpy, cmap);
   XCloseDisplay(dpy);
   LOG_INFO("%s: clear.", getName());
   do_update = &Module::empty_update;
   return 0;
 }
