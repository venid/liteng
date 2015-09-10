
#pragma once

#include "object.h"

class Data;

class Texture : public Object
 { private:
    unsigned int m_target;
    unsigned int m_gl;
    unsigned int m_init;

   public:
    Texture(const char *theName);
    ~Texture ();

    bool init();
    void clear();

    unsigned int getTarget() const { return m_target; }
    unsigned int getGL() const { return m_gl; }

    static Meta::Base Instance;
 };
