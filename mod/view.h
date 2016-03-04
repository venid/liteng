#ifndef VIEW_H_INCLUDE
#define VIEW_H_INCLUDE

#include <GL/glx.h>    /* this includes the necessary X headers */
#include "module.h"

namespace Lua { class State; }

struct WINRECT
 {
   int width, height;
   int posX, posY;
 };

class View : public Module
 { private:
    Display   *dpy;
    Window     win;
    GLXContext ctx;
    Colormap   cmap;
    Atom wmDeleteMessage;

    WINRECT winRect;

    bool init(Lua::State &lua);
    void set_var();
    void render();

    int init_update(double);
    int run_update(double);

   public:
    View(const char* Name);
    ~View() {}

    void connectMsg();
    void clear(Object*, int);
    void add(Object*, int);

    static Object* Create(const char* Name, Lua::State* lua)
     { View *view = new View(Name);
       view->init(*lua);
       return view;
     }

    static Meta::Base Instance;
 };

#endif // VIEW_H_INCLUDE 
