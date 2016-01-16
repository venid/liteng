
#include "input.h"
#include "log.h"
#include <X11/XKBlib.h>
#include <memory.h>
#include <map>

enum KeyMap
 { KEY_0               = 48,
   KEY_1               = 49,
   KEY_2               = 50,
   KEY_3               = 51,
   KEY_4               = 52,
   KEY_5               = 53,
   KEY_6               = 54,
   KEY_7               = 55,
   KEY_8               = 56,
   KEY_9               = 57,

   KEY_A               = 97,
   KEY_B               = 98,
   KEY_C               = 99,
   KEY_D               = 100,
   KEY_E               = 101,
   KEY_F               = 102,
   KEY_G               = 103,
   KEY_H               = 104,
   KEY_I               = 105,
   KEY_J               = 106,
   KEY_K               = 107,
   KEY_L               = 108,
   KEY_M               = 109,
   KEY_N               = 110,
   KEY_O               = 111,
   KEY_P               = 112,
   KEY_Q               = 113,
   KEY_R               = 114,
   KEY_S               = 115,
   KEY_T               = 116,
   KEY_U               = 117,
   KEY_V               = 118,
   KEY_W               = 119,
   KEY_X               = 120,
   KEY_Y               = 121,
   KEY_Z               = 122,

   KEY_F1              = 190,
   KEY_F2              = 191,
   KEY_F3              = 192,
   KEY_F4              = 193,
   KEY_F5              = 194,
   KEY_F6              = 195,
   KEY_F7              = 196,
   KEY_F8              = 197,
   KEY_F9              = 198,
   KEY_F10             = 199,
   KEY_F11             = 200,
   KEY_F12             = 201,

        //KEY_BACKSPACE       = 8,
        //KEY_TAB             = 9,
        //KEY_ENTER           = 13,
        //KEY_CAPSLOCK        = 20,
        //KEY_ESCAPE          = 27,
        //KEY_SPACE           = 32,
        //KEY_APP             = 93,
        //KEY_SEMICOLON       = 186,
        //KEY_ADD             = 187,
        //KEY_COMMA           = 188,
        //KEY_SUBTRACT        = 189,
        //KEY_PERIOD          = 190,
        //KEY_FWDSLASH        = 191,
        //KEY_TILDE           = 192,
        //KEY_LBRACKET        = 219,
        //KEY_BACKSLASH       = 220,
        //KEY_RBRACKET        = 221,
        //KEY_APOSTROPHE      = 222,

        //KEY_SHIFT           = 16,
        //KEY_CTRL            = 17,
        //KEY_LWIN            = 91,
        //KEY_RWIN            = 92,
        //KEY_LALT            = 164,
        //KEY_RALT            = 165,

        //KEY_INSERT          = 45,
        //KEY_DELETE          = 46,
        //KEY_HOME            = 36,
        //KEY_END             = 35,
        //KEY_PAGEUP          = 33,
        //KEY_PAGEDOWN        = 34,

        //KEY_UP              = 38,
        //KEY_DOWN            = 40,
        //KEY_RIGHT           = 39,
        //KEY_LEFT            = 37,

        //KEY_NUMLOCK         = 144,
        //KEY_NUMPAD_DIVIDE   = 111,
        //KEY_NUMPAD_MULTIPLY = 106,
        //KEY_NUMPAD_SUBTRACT = 109,
        //KEY_NUMPAD_ADD      = 107,
        //KEY_NUMPAD_DECIMAL  = 110,
        //KEY_NUMPAD_0        = 96,
        //KEY_NUMPAD_1        = 97,
        //KEY_NUMPAD_2        = 98,
        //KEY_NUMPAD_3        = 99,
        //KEY_NUMPAD_4        = 100,
        //KEY_NUMPAD_5        = 101,
        //KEY_NUMPAD_6        = 102,
        //KEY_NUMPAD_7        = 103,
        //KEY_NUMPAD_8        = 104,
        //KEY_NUMPAD_9        = 105,
    };

Keyboard *theKeyboard = nullptr;
Mousedevice *theMouse = nullptr;

Keyboard :: Keyboard() : m_atf(ATOMIC_FLAG_INIT)
 { m_dpy = nullptr;
   m_focus = false;
   memset(m_keys, 0, 256);
 }

void Keyboard :: setDisplay(Display *dpy)
 { while(m_atf.test_and_set()) {}
   m_dpy = dpy;
   m_atf.clear();
 }

void Keyboard :: setFocus(bool fs)
 { while(m_atf.test_and_set()) {}
   m_focus = fs;
   m_atf.clear();
 }

void Keyboard :: update()
 { char key[32];
   unsigned char szKeysym;

   memset(m_keys, 0, 256);
   while(m_atf.test_and_set()) {}
   if(m_dpy && m_focus)
    { XQueryKeymap(m_dpy, key);
      for( uint32_t i = 0; i < 32; i++ )
       { const char& currentChar = key[i];
         // iterate over each bit and check if the bit is set
         for( uint32_t j = 0; j < 8; j++ )
          { bool isKeyPressed = ((1 << j) & currentChar) != 0;
            if( isKeyPressed )
             { KeyCode keyCode = (i * sizeof(char) * 8) + j;
               szKeysym = XkbKeycodeToKeysym(m_dpy, keyCode, 0, 0);
               m_keys[szKeysym] = 1;
              // LOG_SPAM("Input::Keyboard: press button %i", szKeysym);
             }
          }
       }
    }
   m_atf.clear();
 }

Keyboard& Keyboard :: instance()
 { return *theKeyboard; }

void Keyboard :: Init()
 { if(theKeyboard == nullptr) theKeyboard = new Keyboard(); }

void Keyboard :: Clear()
 { if(theKeyboard) delete theKeyboard; }

// --------------------------------------------------------------------------------------

Mousedevice :: Mousedevice() : m_atf(ATOMIC_FLAG_INIT)
 { m_dpy = nullptr;
   m_win = 0;
   m_focus = false;

   m_wheel_delta = 0.2f;
   m_wheel = 0.f;

   m_posAbsolute = {0, 0};
   m_posRelative = {0.f, 0.f};

   m_width = 0;
   m_height = 0;
 }

void Mousedevice :: setDisplay(Display *dpy)
 { while(m_atf.test_and_set()) {}
   m_dpy = dpy;
   m_atf.clear();
 }

void Mousedevice :: setWindow(Window win)
 { Window root;
   int x, y;
   unsigned border_width, depth;

   while(m_atf.test_and_set()) {}
   m_win = win;
   if(m_win != 0)
    XGetGeometry(m_dpy, m_win, &root, &x, &y, &m_width, &m_height, &border_width, &depth);
   m_atf.clear();
 }

void Mousedevice :: setFocus(bool fs)
 { while(m_atf.test_and_set()) {}
   m_focus = fs;
   m_atf.clear();
 }

void Mousedevice :: setWheelFlag(int flag)
 { while(m_atf.test_and_set()) {}
   m_wheel_flag = flag;
   m_atf.clear();
 }

void Mousedevice :: update()
 { Window child, root;
   int root_x, root_y, win_x, win_y;
   unsigned mask;

   m_flags = 0;
   while(m_atf.test_and_set()) {}
   if(m_dpy && m_win && m_focus)
    { XQueryPointer(m_dpy, m_win, &root, &child, &root_x, &root_y, &win_x, &win_y, &mask);
      m_wheel = m_wheel_flag * m_wheel_delta;
      if(m_wheel_flag != 0)
       { m_flags |= M_WHEELE;
         m_wheel_flag = 0;
         LOG_SPAM("Input::Mowse: wheel event");
       }
      m_atf.clear();
      m_posRelative.x = win_x - m_posAbsolute.x;
      m_posRelative.y = win_y - m_posAbsolute.y;
      m_posAbsolute.x = win_x;
      m_posAbsolute.y = win_y;
      if((m_posRelative.x != 0.f)||(m_posRelative.y != 0.f)) m_flags |= M_MOVE;
      if((mask & Button1Mask) != 0) m_flags |= M_LEFT;
      if((mask & Button2Mask) != 0) m_flags |= M_MIDDLE;
      if((mask & Button3Mask) != 0) m_flags |= M_RIGHT;
    }
   else  m_atf.clear();
 }

Mousedevice& Mousedevice :: instance()
 { return *theMouse; }

void Mousedevice :: Init()
 { if(theMouse == nullptr) theMouse = new Mousedevice(); }

void Mousedevice :: Clear()
 { if(theMouse) delete theMouse; }
