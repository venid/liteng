
#pragma once

#include <atomic>
#include<X11/Xlib.h>


class Keyboard
 { private:
    Display *m_dpy;
    std::atomic_flag m_atf;
    bool m_focus;

    char m_keys[256];

    Keyboard();
    ~Keyboard() {}

   public:
    void setDisplay(Display *dpy);
    void setFocus(bool);

    bool isKeyState(unsigned char key)
     { return m_keys[key];}
    void update();

    static Keyboard& instance();
    static void Init();
    static void Clear();

 };

#define M_MOVE   0x01
#define M_WHEELE 0x02
#define M_LEFT   0x04
#define M_RIGHT  0x08
#define M_MIDDLE 0x10
#define M_CURSOR 0x20

class Mousedevice
 { private:
    Display *m_dpy;
    Window   m_win;
    std::atomic_flag m_atf;

    char m_flags;
    bool m_focus;

    int   m_wheel_flag;
    float m_wheel;
    float m_wheel_delta;

    struct
     { int x;
       int y;
     } m_posAbsolute;

    struct
     { float x;
       float y;
     } m_posRelative;

    Mousedevice();
    ~Mousedevice() {}

   public:
    void setDisplay(Display *dpy);
    void setWindow(Window win);
    void setFocus(bool fs);
    void setWheelFlag(int flag);
    void update();

    bool isState(char flag)
     { if((m_flags & flag) == flag) return true;
       else return false;
     }

    void getPosRelative(float& x, float& y)
     { x = m_posRelative.x;
       y = m_posRelative.y;
     }

    void getPosAbsolute(int& x, int& y)
     { x = m_posAbsolute.x;
       y = m_posAbsolute.y;
     }

    static Mousedevice& instance();
    static void Init();
    static void Clear();
 };
